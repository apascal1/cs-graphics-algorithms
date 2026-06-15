#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;


class Camera{
    public:

    Eigen:: Vector3d from, at, up;
    Camera(){
        m_angle = 0.0;
        m_width = 0;
        m_height = 0;
        m_hither = 0.0;
    }
    Camera(double angle, int width, int height, double hither){
        m_angle = angle;
        m_width = width;
        m_height = height;
        m_hither = hither;
    } 
    void setAngle(double angle) {
        m_angle = angle;
    }

    void setHeight(int height){
        m_height = height;
    }

    void setWidth(int width){
        m_width = width;
    }
    
    void setHither(double hither){
        m_hither = hither;
    }
    double getAngle(){
        return m_angle;
    }

    int getHeight(){
        return m_height;
    }

    int getWidth(){
        return m_width;
    }

    double getHither(){
        return m_hither;
    }

    private:
    double m_angle;
    int m_width, m_height;
    double m_hither;
};

class Light{
    public:
    
    Light() {m_x = 0; m_y = 0; m_z = 0; m_position = Eigen::Vector3d(0,0,0);}
    Light(double x,double y,double z){ m_position = Eigen::Vector3d(x,y,z); }

    const Eigen::Vector3d& getPosition() const {return m_position;}

    private:
    double m_x;
    double m_y;
    double m_z;
    Eigen::Vector3d m_position;
};

class Material {
    public: 
    Material(){
        m_color = Eigen::Vector3d(1,1,1);
        m_Kd = 1.0;
        m_Ks = 0.0;
        m_e = 32.0;
        m_Kt = 0.0;
        m_ir = 1.0;
    }
    Material(double Kd, double Ks, double e, double Kt, double ir, Eigen::Vector3d color ){
        m_Kd = Kd;
        m_Ks = Ks;
        m_e = e;
        m_Kt = Kt;
        m_ir = ir;
        m_color = color;

    }
    double getKd() const { return m_Kd;}
    double getKs() const { return m_Ks;}
    double getE() const { return m_e;}
    const Eigen::Vector3d& getColor() const { return m_color; }


    void setKd(double Kd){  m_Kd = Kd;}
    void setKs(double Ks){ m_Ks = Ks; }
    void setE(double e){ m_e = e; }
    void setColor(const Eigen::Vector3d& color){ m_color = color;}
    
    


    private:
        double m_Kd;             // diffuse strength
        double m_Ks;             // specular/reflect strength
        double m_e;              // shininess exponent
        double m_Kt;             //skipping these
        double m_ir;             //skipping these
        Eigen::Vector3d m_color;
};


class Triangle {
    public:
    Eigen::Vector3d v0, v1, v2;
    Eigen::Vector3d n0, n1, n2;
    Eigen::Vector3d m_color;
    bool hasVertexNormals = false;
    Material mat;
    Eigen::Vector3d faceN; 

    

    Triangle(){
        v0 = Eigen::Vector3d::Zero();
        v1 = Eigen::Vector3d::Zero();
        v2 = Eigen::Vector3d::Zero();

        m_color = Eigen::Vector3d::Zero();
    }

    Triangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &color){
        v0 = a;
        v1 = b;
        v2 = c;
        m_color = color;
    }

    void setVertices(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c){
        v0 = a;
        v1 = b;
        v2 = c;
        faceN = (v1-v0).cross(v2-v0).normalized();

    }

    void setNormals(const Eigen::Vector3d& a,
                    const Eigen::Vector3d& b,
                    const Eigen::Vector3d& c) {
        n0 = a.normalized(); 
        n1 = b.normalized();
         n2 = c.normalized();
        hasVertexNormals = true;
    }

    void setColor(const Eigen::Vector3d & color){
        m_color = color;
    }

    void setMaterial(const Material & material){
        mat = material;
    }

};

// Convert NDC to pixel coords and keep z
void ndcToPixel(const Eigen::Vector3d& ndc, int W, int H, int& sx, int& sy, double& z){
            double fx = (ndc.x() * 0.5 + 0.5) * double(W);
            double fy = (1.0 - ndc.y()) * 0.5 * double(H);
            sx = static_cast<int>(fx);
            sy = static_cast<int>(fy);
            z = ndc.z(); 
}
//helper
double edgeFunction(int ax, int ay, int bx, int by, int px, int py) {
    return (double)(bx - ax) * (py - ay) - (double)(by - ay) * (px - ax);
}



int main() {
    Eigen::Vector3d backgroundcolor; //for our x,y,z plane
    vector<Triangle> triangles; //holds the triangles to hit
    vector<Light> lights;
    Material currentMat;
    Camera camera;
    

    ifstream file("teapot-3.nff");
    if(!file){
        cout << "file not found" << endl;
    }

    string line;
   
    while(getline(file, line)){
        if(line.empty()){
            continue; //if our file is empty
        }
                
        stringstream ss(line);
        char ch = line[0];
        string keyword;
        ss >> keyword;
        

        switch(ch){
            case 'b':{
            double r , g , b; //declare our colors
            ss >> r >> g >> b; //read them in line by line
            backgroundcolor = Eigen::Vector3d(r,g,b); //store in eigenvector
            break;
            }       
            case 'v': {
                for (int t = 0; t < 6; ++t) {
                    if (!getline(file, line)) break;
                    if (line.empty()) { --t; continue; }

                    stringstream vs(line);
                    string key;
                    vs >> key;

                    if (key == "from") {
                        double a,b,c; vs >> a >> b >> c;
                        camera.from = Eigen::Vector3d(a,b,c);
                    } else if (key == "at") {
                        double a,b,c; vs >> a >> b >> c;
                        camera.at = Eigen::Vector3d(a,b,c);
                    } else if (key == "up") {
                        double a,b,c; vs >> a >> b >> c;
                        camera.up = Eigen::Vector3d(a,b,c);
                    } else if (key == "angle") {
                        double ang; vs >> ang;
                        camera.setAngle(ang);
                    } else if (key == "hither") {
                        double hit; vs >> hit;
                        camera.setHither(hit);
                    } else if (key == "resolution") {
                        int w,h; vs >> w >> h;
                        camera.setWidth(w);
                        camera.setHeight(h);
                    } else {
                        --t;
                        continue;
                    }
                }
                break;
            }

            case 'l': {
            double l, m, n; ss >> l >> m >> n; 
            lights.emplace_back(l,m,n);
            break;
            }

            case 'f':{ 
            double r,g,b, Kd,Ks,e, Kt,ir;
            ss >> r >> g >> b >> Kd >> Ks >> e >> Kt >> ir;
            currentMat.setColor(Eigen::Vector3d(r, g, b));
            currentMat.setE(e);
            currentMat.setKs(Ks);
            currentMat.setKd(Kd);
            break;}

            case 'p':{
            int n; ss >> n;
            vector <Eigen::Vector3d> verts; verts.reserve(n);
            vector<Eigen::Vector3d> norms;
            
            for(int i = 0 ; i < n; ++i){
                getline(file,line);
                stringstream vs(line);
                double x,y,z;
                if(keyword == "pp"){
                    double nx, ny, nz;
                    vs >> x >> y >> z >> nx >> ny >> nz;
                    Eigen::Vector3d n(nx, ny, nz);
                    norms.push_back(n.normalized());

                } else {
                    vs >> x >> y >> z;
                }
                verts.emplace_back(x,y,z);
            }
                for(int i = 1; i <= n-2; ++i){
                Triangle tri;
                tri.setVertices(verts[0], verts[i], verts[i+1]);
                if(keyword == "pp" && !norms.empty()){
                    tri.setNormals(norms[0], norms[i], norms[i+1]);
                    tri.hasVertexNormals = true;
                } else {
                    Eigen::Vector3d v0 = verts[0];
                    Eigen::Vector3d v1 = verts[i];
                    Eigen::Vector3d v2 = verts[i+1];
                    tri.faceN = (v1 - v0).cross(v2 - v0).normalized();
                    tri.hasVertexNormals = false;
                }
                tri.setMaterial(currentMat);
                triangles.push_back(tri);
            }
        
        break;
        } 
        }
        
    }
    file.close();

    double fov = camera.getAngle() * M_PI / 180.0;
    double aspect = double(camera.getWidth()) / double(camera.getHeight());
    double zn = camera.getHither();
    double zf = 1000.0; // some far plane

    double f = 1.0 / tan(fov / 2.0);


    const int W = camera.getWidth();
    const int H = camera.getHeight();

    // Per-pixel buffers
    vector<Eigen::Vector3d> colorBuffer(W * H);
    vector<double>          depthBuffer(W * H);

    // Clear; once per frame
    for (int i = 0; i < W*H; ++i) {
        colorBuffer[i] = backgroundcolor;                
        depthBuffer[i] = numeric_limits<double>::infinity(); 
    }
    Eigen::Vector3d w = (camera.from - camera.at).normalized(); // camera direction
    Eigen::Vector3d u = (camera.up.cross(w)).normalized();      // right
    Eigen::Vector3d v = w.cross(u);                             // up 

    Eigen::Matrix4d M_cam;
    M_cam <<
    u.x(), u.y(), u.z(), -u.dot(camera.from),
    v.x(), v.y(), v.z(), -v.dot(camera.from),
    w.x(), w.y(), w.z(), -w.dot(camera.from),
    0, 0, 0, 1;

    Eigen::Matrix4d M_per;
    M_per <<
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zf + zn) / (zn - zf), (2 * zf * zn) / (zn - zf),
        0, 0, -1, 0;
    
    Eigen::Matrix4d M = M_per * M_cam;

    for (const Triangle& tri : triangles) {

        //vertices -> homogeneous
        Eigen::Vector4d Pw0(tri.v0.x(), tri.v0.y(), tri.v0.z(), 1.0);
        Eigen::Vector4d Pw1(tri.v1.x(), tri.v1.y(), tri.v1.z(), 1.0);
        Eigen::Vector4d Pw2(tri.v2.x(), tri.v2.y(), tri.v2.z(), 1.0);


        Eigen::Vector4d Pc0 = M * Pw0;
        Eigen::Vector4d Pc1 = M * Pw1;
        Eigen::Vector4d Pc2 = M * Pw2;

    
        Eigen::Vector3d ndc0(Pc0.x()/Pc0.w(), Pc0.y()/Pc0.w(), Pc0.z()/Pc0.w());
        Eigen::Vector3d ndc1(Pc1.x()/Pc1.w(), Pc1.y()/Pc1.w(), Pc1.z()/Pc1.w());
        Eigen::Vector3d ndc2(Pc2.x()/Pc2.w(), Pc2.y()/Pc2.w(), Pc2.z()/Pc2.w());

        int sx0, sy0, sx1, sy1, sx2, sy2;
        double z0, z1, z2; 
        ndcToPixel(ndc0, W, H, sx0, sy0, z0);
        ndcToPixel(ndc1, W, H, sx1, sy1, z1);
        ndcToPixel(ndc2, W, H, sx2, sy2, z2);

        // 4. Bounding Box Calculation
        int x_min = max(0, min({sx0, sx1, sx2}));
        int y_min = max(0, min({sy0, sy1, sy2}));
        int x_max = min(W - 1, max({sx0, sx1, sx2}));
        int y_max = min(H - 1, max({sy0, sy1, sy2}));

        // Area
        // Calculated once per triangle
        double denom = edgeFunction(sx0, sy0, sx1, sy1, sx2, sy2);
        const double EPSILON = 1e-9;
        if (abs(denom) < EPSILON) continue; 

        //  for shading mode
        Eigen::Vector3d lightPos(10, 10, 10);
        Eigen::Vector3d viewPos = camera.from;
        bool smoothShading = true; // true = smooth shading

        for (int y = y_min; y <= y_max; ++y) {
            for (int x = x_min; x <= x_max; ++x) {
                // Calculate barycentric coordinates
                double num2 = edgeFunction(sx0, sy0, sx1, sy1, x, y);
                double num0 = edgeFunction(sx1, sy1, sx2, sy2, x, y);
                double num1 = edgeFunction(sx2, sy2, sx0, sy0, x, y);

                double alpha = num0 / denom;
                double beta = num1 / denom;
                double gamma = num2 / denom;
                
                if (alpha >= -EPSILON && beta >= -EPSILON && gamma >= -EPSILON) {
                    // Interpolated depth
                    double interpolated_z = alpha * z0 + beta * z1 + gamma * z2;

                    int buffer_idx = y * W + x;
                    if (interpolated_z < depthBuffer[buffer_idx]) {
                        // Interpolated vertex position
    
                        Eigen::Vector3d vertPos = alpha * tri.v0 + beta * tri.v1 + gamma * tri.v2;

                        // Decide normal based on shading mode
                        Eigen::Vector3d interpNormal;
                        if (smoothShading && tri.hasVertexNormals) {
                            // Smooth shading and interpolate normals
                            interpNormal = (alpha * tri.n0 + beta * tri.n1 + gamma * tri.n2).normalized();
                        } else {
                            interpNormal = tri.faceN;
                        }

                        // Lighting calculations
                        Eigen::Vector3d L = (lightPos - vertPos).normalized();
                        Eigen::Vector3d V = (viewPos - vertPos).normalized();
                        Eigen::Vector3d R = (2 * interpNormal.dot(L) * interpNormal - L).normalized();

                        // Material and light coef.
                        Eigen::Vector3d ka(0.1, 0.1, 0.1);
                        Eigen::Vector3d kd = tri.mat.getColor();
                        Eigen::Vector3d ks(0.2, 0.2, 0.2); 
                        double shininess = tri.mat.getE();

                        Eigen::Vector3d ambient = ka.cwiseProduct(Eigen::Vector3d(1,1,1));
                        double NdotL = max(interpNormal.dot(L), 0.0);
                        Eigen::Vector3d diffuse = kd * NdotL;

                        double RdotV = max(R.dot(V), 0.0);
                        Eigen::Vector3d specular = ks * pow(RdotV, shininess);

                        specular *= 0.67; //Play around if you're grading it changes the brightness, higher = shinnier

                        // Final color
                        Eigen::Vector3d color = ambient + diffuse + specular;

                        color = color.cwiseMin(1.0).cwiseMax(0.0);
                    

                        //  buffers
                        depthBuffer[buffer_idx] = interpolated_z;
                        colorBuffer[buffer_idx] = color;
                    }
                }
            }
        }
    } 

//output
 ofstream out("output.ppm");
    out << "P3\n" << W << " " << H << "\n255\n";
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const Eigen::Vector3d& c = colorBuffer[y * W + x];
            int R = std::min(255, std::max(0, int(c(0) * 255.0 + 0.5)));
            int G = std::min(255, std::max(0, int(c(1) * 255.0 + 0.5)));
            int B = std::min(255, std::max(0, int(c(2) * 255.0 + 0.5)));
            out << R << " " << G << " " << B << "\n";
        }
    }
    out.close(); 

return 0;


}
