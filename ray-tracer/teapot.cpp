#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
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
    Eigen::Vector3d m_color;
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

    void setColor(const Eigen::Vector3d & color){
        m_color = color;
    }

    void setMaterial(const Material & material){
        mat = material;
    }

};


struct ray {
    Eigen::Vector3d origin{0,0,0};
    Eigen::Vector3d dir{0,0,-1};   // must be unit set 
    int depth = 0;                  // 0 for primary rays

    ray() = default;
    ray(const Eigen::Vector3d& o, const Eigen::Vector3d& d, int dep=0)
        : origin(o), dir(d.normalized()), depth(dep) {} 
};

struct hitbox {
    bool hit = false;
    double t = std::numeric_limits<double>::infinity(); // closest point
    Eigen::Vector3d p{0,0,0};   // hit point
    Eigen::Vector3d n{0,0,1};   // unit normal at hit
    const Material* mat = nullptr; // pointer to mat class for material of the hit triangle
};


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
            
            for(int i = 0 ; i < n; ++i){
                getline(file,line);
                stringstream vs(line);
                double x,y,z;
                if(keyword == "pp"){
                    double nx,ny,nz;
                    vs >> x >> y >> z >> nx >> ny >> nz;
                } else {
                    vs >> x >> y >> z;
                }
                verts.emplace_back(x,y,z);
            }

            for(int i = 1; i <= n-2; ++i){
                Triangle tri;
                tri.setVertices(verts[0], verts[i], verts[i+1]);
                tri.setMaterial(currentMat);
                triangles.push_back(tri);
        }
        
        break;
        } 
        }
        
    }
    file.close();

        int W = camera.getWidth();
        int H = camera.getHeight();

        Eigen::Vector3d w = ((camera.from - camera.at)).normalized();
        Eigen::Vector3d u = ((camera.up).cross(w)).normalized(); //getting the formulas to set the scene
        Eigen::Vector3d v = w.cross(u);
        

        double aspect = double(camera.getWidth()) / double(camera.getHeight());
        double plane_width  = 2.0 * tan(camera.getAngle() * M_PI / 360.0);
        double plane_height = plane_width / aspect; 


        Eigen::Vector3d* image = new Eigen::Vector3d[W * H];
        const double EPS = 1e-6;

        for (int j = 0; j < H; ++j) {
            for (int i = 0; i < W; ++i) {

                // primary ray through pixel (i,j)
                double u_s = ((i + 0.5) / double(W)) * 2.0 - 1.0;
                double v_s = 1.0 - ((j + 0.5) / double(H)) * 2.0;
                double x = u_s * (plane_width  * 0.5);
                double y = v_s * (plane_height * 0.5);
                Eigen::Vector3d ray_dir = (x * u + y * v - w).normalized();

                // find closest triangle
                double closest = std::numeric_limits<double>::infinity();
                int    hitIdx  = -1;
                double hitT    = 0.0;

                for (size_t k = 0; k < triangles.size(); ++k) {
                    const Triangle& tri = triangles[k];
                    Eigen::Vector3d e1 = tri.v1 - tri.v0;
                    Eigen::Vector3d e2 = tri.v2 - tri.v0;

                    Eigen::Matrix3d M;
                    M.col(0) = e1; M.col(1) = e2; M.col(2) = -ray_dir;

                    Eigen::Vector3d X = M.colPivHouseholderQr().solve(camera.from - tri.v0);
                    double a = X(0), b = X(1), t = X(2);

                    if (a >= 0.0 && b >= 0.0 && a + b <= 1.0 && t > 0.0 && t < closest) {
                        closest = t; hitIdx = int(k); hitT = t;
                    }
                }

                Eigen::Vector3d finalColor = backgroundcolor;

             if (hitIdx != -1) {
            const Triangle& tri = triangles[hitIdx];

            // hit point and normal (but flip if backfacing)
            Eigen::Vector3d P = camera.from + hitT * ray_dir;
            Eigen::Vector3d N = tri.faceN;
            if (N.dot(ray_dir) > 0.0) N = -N;

            // diffuse + hard shadows
            Eigen::Vector3d shaded = Eigen::Vector3d::Zero();
            double lightScale = lights.empty() ? 1.0
                                            : 1.0 / std::sqrt(double(lights.size()));

            for (size_t li = 0; li < lights.size(); ++li) {
                Eigen::Vector3d toL   = lights[li].getPosition() - P;
                double          Ldist = toL.norm();
                if (Ldist <= EPS) continue;
                Eigen::Vector3d Ldir  = toL / Ldist;

                double diff = N.dot(Ldir);
                if (diff <= 0.0) continue;

                // shadow test
                bool blocked = false;
                Eigen::Vector3d Sorig = P + N * EPS;
                for (size_t m = 0; m < triangles.size(); ++m) {
                    if (m == (size_t)hitIdx) continue;
                    const Triangle& T = triangles[m];
                    Eigen::Vector3d e1s = T.v1 - T.v0;
                    Eigen::Vector3d e2s = T.v2 - T.v0;

                    Eigen::Matrix3d Ms;
                    Ms.col(0) = e1s; Ms.col(1) = e2s; Ms.col(2) = -Ldir;

                    Eigen::Vector3d Xs = Ms.colPivHouseholderQr().solve(Sorig - T.v0);
                    double as = Xs(0), bs = Xs(1), ts = Xs(2);

                    if (as >= 0.0 && bs >= 0.0 && as + bs <= 1.0 && ts > EPS && ts < Ldist - EPS) {
                        blocked = true; break;
                    }
                }
                if (!blocked) {
                    shaded += tri.mat.getKd() * tri.mat.getColor() * diff * lightScale;
                }
            }

            if (lights.empty()) shaded = tri.mat.getColor();

            // start with local (diffuse) shading
            Eigen::Vector3d local = shaded.array().max(0.0).min(1.0).matrix();
            finalColor = local;
            
            if (tri.mat.getKs() > 0.0) {
            // reflection ray
            Eigen::Vector3d R = (ray_dir - 2.0 * ray_dir.dot(N) * N).normalized();
            Eigen::Vector3d Rorig = P + N * EPS; // switch to P + R * EPS if bad reflection

            //find closest along reflection
            double closest2 = std::numeric_limits<double>::infinity();
            int    hit2 = -1;
            double t2 = 0.0;

            for (size_t m = 0; m < triangles.size(); ++m) {
                const Triangle& T2 = triangles[m];
                Eigen::Vector3d e1 = T2.v1 - T2.v0;
                Eigen::Vector3d e2 = T2.v2 - T2.v0;

                Eigen::Matrix3d M2;
                M2.col(0) = e1; M2.col(1) = e2; M2.col(2) = -R;

                Eigen::Vector3d X2 = M2.colPivHouseholderQr().solve(Rorig - T2.v0);
                double a2 = X2(0), b2 = X2(1), t = X2(2);

                if (a2 >= 0.0 && b2 >= 0.0 && a2 + b2 <= 1.0 && t > EPS && t < closest2) {
                    closest2 = t; hit2 = int(m); t2 = t;
                }
            }

           Eigen::Vector3d refl = backgroundcolor;
            if (hit2 != -1) {
                const Triangle& T2 = triangles[hit2];

                Eigen::Vector3d P2 = Rorig + t2 * R;
                Eigen::Vector3d N2 = T2.faceN;
                if (N2.dot(R) > 0.0) N2 = -N2;

                Eigen::Vector3d shade2 = Eigen::Vector3d::Zero();
                double lightScale2 = lights.empty() ? 1.0
                                                    : 1.0 / std::sqrt(double(lights.size()));

                                                    

                // simple lighting on the reflected hit
                Eigen::Vector3d V2 = (-R).normalized();
                for (const auto& Lg : lights) {
                    Eigen::Vector3d L = (Lg.getPosition() - P2).normalized();

                    double nl = N2.dot(L);
                    if (nl <= 0.0) continue;          

                    double diff2 = nl;
                    Eigen::Vector3d H2 = (L + V2).normalized();
                    
                    double shininess = std::max(1.0, tri.mat.getE() * 16.0);   //for lighting trying to make it brighter 

                    double spec2 = std::pow(std::max(0.0, N2.dot(H2)), shininess);

                    shade2 += T2.mat.getKd() * T2.mat.getColor() * diff2 * lightScale2;
                    shade2 += T2.mat.getKs() * spec2 * lightScale2 * Eigen::Vector3d::Ones();
                }
  

                if (lights.empty()) shade2 = T2.mat.getColor();
                refl = shade2.array().max(0.0).min(1.0).matrix();
            }

            // mix reflection into the local shading
            double ks = tri.mat.getKs();
            finalColor = (local + tri.mat.getKs() * refl).array().max(0.0).min(1.0).matrix();

    }
} 

        // write pixel
        image[j * W + i] = finalColor;
}


        // write PPM (P3)
        std::ofstream out("output.ppm");
        out << "P3\n" << W << " " << H << "\n255\n";
        for (int j = 0; j < H; ++j) {
            for (int i = 0; i < W; ++i) {
                Eigen::Vector3d c = image[j * W + i];
                int R = std::min(255, std::max(0, int(c(0) * 255.0 + 0.5)));
                int G = std::min(255, std::max(0, int(c(1) * 255.0 + 0.5)));
                int B = std::min(255, std::max(0, int(c(2) * 255.0 + 0.5)));
                out << R << " " << G << " " << B << "\n";
            }
        }
        out.close();


    }
       
return 0;
}


 
