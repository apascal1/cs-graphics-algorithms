#define cimg_display 0
#include "CImg.h"
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <climits>
using namespace std;
using namespace cimg_library;

// Energy function 
double computePixelEnergy(Eigen::Vector3d* image, int width, int height, int x, int y) {
    double total_energy = 0.0;
    
    // Compute gradient for each channel (L, a, b)
    for (int c = 0; c < 3; c++) {
        double dx, dy;
        
        // x-gradient (centered difference)
        if (x == 0) {
            dx = image[(x+1)*height + y][c] - image[x*height + y][c];
        } else if (x == width - 1) {
            dx = image[x*height + y][c] - image[(x-1)*height + y][c];
        } else {
            dx = (image[(x+1)*height + y][c] - image[(x-1)*height + y][c]) / 2.0;
        }
        
        // y-gradient (centered difference)
        if (y == 0) {
            dy = image[x*height + (y+1)][c] - image[x*height + y][c];
        } else if (y == height - 1) {
            dy = image[x*height + y][c] - image[x*height + (y-1)][c];
        } else {
            dy = (image[x*height + (y+1)][c] - image[x*height + (y-1)][c]) / 2.0;
        }
        
        total_energy += abs(dx) + abs(dy);
    }
    
    return total_energy;
}

// Find vertical seam using DP
vector<int> findVerticalSeam(Eigen::Vector3d* image, int width, int height) {
    // Compute energy map
    vector<double> energy(width * height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            energy[x * height + y] = computePixelEnergy(image, width, height, x, y);
        }
    }
    
    // DP tables
    vector<double> dp(width * height);
    vector<int> parent(width * height);
    
    // Initialize first column (y = 0)
    for (int x = 0; x < width; x++) {
        dp[x * height + 0] = energy[x * height + 0];
    }
    
    // Fill DP table
    for (int y = 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = x * height + y;
            
            // Check three parents from previous row
            double min_energy = dp[x * height + (y-1)];
            int best_parent = x;
            
            if (x > 0) {
                double left_energy = dp[(x-1) * height + (y-1)];
                if (left_energy < min_energy) {
                    min_energy = left_energy;
                    best_parent = x - 1;
                }
            }
            
            if (x < width - 1) {
                double right_energy = dp[(x+1) * height + (y-1)];
                if (right_energy < min_energy) {
                    min_energy = right_energy;
                    best_parent = x + 1;
                }
            }
            
            dp[idx] = energy[idx] + min_energy;
            parent[idx] = best_parent;
        }
    }
    
    // Backtrack to find seam
    vector<int> seam(height);
    
    // Find minimum in last row
    int min_x = 0;
    for (int x = 1; x < width; x++) {
        if (dp[x * height + (height-1)] < dp[min_x * height + (height-1)]) {
            min_x = x;
        }
    }
    
    seam[height-1] = min_x;
    for (int y = height - 1; y > 0; y--) {
        seam[y-1] = parent[seam[y] * height + y];
    }
    
    return seam;
}

// Remove vertical seam from image
void removeVerticalSeam(Eigen::Vector3d*& image, int& width, int height, const vector<int>& seam) {
    // Create new array with reduced width
    Eigen::Vector3d* new_image = new Eigen::Vector3d[(width-1) * height];
    
    for (int y = 0; y < height; y++) {
        int seam_x = seam[y];
        
        // Copy columns before seam
        for (int x = 0; x < seam_x; x++) {
            new_image[x * height + y] = image[x * height + y];
        }
        
        // Copy columns after seam (shift by 1)
        for (int x = seam_x; x < width - 1; x++) {
            new_image[x * height + y] = image[(x+1) * height + y];
        }
    }
    
    // Clean up old image and update pointer
    delete[] image;
    image = new_image;
    width--;
}

// Remove horizontal seam (by transposing)
void removeHorizontalSeam(Eigen::Vector3d*& image, int width, int& height, const vector<int>& seam) {
    // Transpose the image (swap rows and columns)
    Eigen::Vector3d* transposed = new Eigen::Vector3d[height * width];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            transposed[y * width + x] = image[x * height + y];
        }
    }
    
    // Remove vertical seam from transposed image
    int temp = width;
    width = height;
    height = temp;
    removeVerticalSeam(transposed, width, height, seam);
    
    // Transpose back
    temp = width;
    width = height;
    height = temp;
    
    delete[] image;
    image = new Eigen::Vector3d[width * height];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            image[x * height + y] = transposed[y * width + x];
        }
    }
    
    delete[] transposed; //still deleting to make sure there's no more memory leaks
}

// Find horizontal seam
vector<int> findHorizontalSeam(Eigen::Vector3d* image, int width, int height) {
    // Transpose, find vertical seam, return it
    Eigen::Vector3d* transposed = new Eigen::Vector3d[height * width];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            transposed[y * width + x] = image[x * height + y];
        }
    }
    
    vector<int> seam = findVerticalSeam(transposed, height, width);
    delete[] transposed;
    return seam;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " input_image output_image output_width output_height\n";
        return 1;
    }
    
    // Load and convert to Lab 
    CImg<double> input(argv[1]);
    CImg<double> lab = input.RGBtoLab();
    
    int original_width = input.width();
    int original_height = input.height();
    int target_width = atoi(argv[3]);
    int target_height = atoi(argv[4]);
    
    // Convert to Eigen array 
    Eigen::Vector3d *image = new Eigen::Vector3d[original_width * original_height];
    for (unsigned int i = 0; i < original_width; i++) {
        for (unsigned int j = 0; j < original_height; j++) {
            //  i = column (x), j = row (y)
            // image[i*height + j] is row-major order
            image[i * original_height + j][0] = lab(i, j, 0);
            image[i * original_height + j][1] = lab(i, j, 1);
            image[i * original_height + j][2] = lab(i, j, 2);
        }
    }
    
    int current_width = original_width;
    int current_height = original_height;
    
    // Remove vertical seams
    while (current_width > target_width) {
        vector<int> seam = findVerticalSeam(image, current_width, current_height);
        removeVerticalSeam(image, current_width, current_height, seam);
    }
    
    // Remove horizontal seams
    while (current_height > target_height) {
        vector<int> seam = findHorizontalSeam(image, current_width, current_height);
        removeHorizontalSeam(image, current_width, current_height, seam);
    }
    
    // Create output image
    CImg<double> output(target_width, target_height, input.depth(), input.spectrum(), 0);
    for (unsigned int i = 0; i < target_width; i++) {
        for (unsigned int j = 0; j < target_height; j++) {
            output(i, j, 0) = image[i * target_height + j][0];
            output(i, j, 1) = image[i * target_height + j][1];
            output(i, j, 2) = image[i * target_height + j][2];
        }
    }
    
    // Convert back to RGB and save
    CImg<double> rgb = output.LabtoRGB();
    string output_name = argv[2];
    
    if (output_name.find(".png") != string::npos || 
        output_name.find(".PNG") != string::npos) {
        rgb.save_png(argv[2]);
    } else {
        rgb.save_jpeg(argv[2]);
    }
    
    delete[] image;
    return 0;
}
