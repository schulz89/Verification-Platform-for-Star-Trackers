// Copyright 2019 Victor Hugo Schulz

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>

#include <opencv2/core.hpp>
#include <boost/qvm/all.hpp>
#include "structures.h"

using namespace std;
using namespace cv;
using namespace boost::qvm;

// Function declarations:
void load_config(string filename_common, string filename_dut, catalog_parameters &cp, grid_parameters &gp, optical_parameters &op, database_parameters &db);
vector<db> initialize_database(catalog_parameters &cp);
vector<db> visible_stars(vector<db> &database, optical_parameters &op);
void next_neighbor(vector<db> &database, database_parameters &db);
vector<db> reference_stars(vector<db> &vstars);
void calculate_descriptors (vector<db> &vstars, vector<db> &rstars, database_parameters &db, grid_parameters &gp);
double fov(double size, optical_parameters op);
double vangle (vec<double,3> p1, vec<double,3> p2);
quat<double> rot_quat (vec<double,3> v1, vec<double,3> v2);
void proper_motion_correction (hip2 entry, int time_y, db *output);
vector<db> rotate3d(vector<db> visible_stars, vector<db> reference_stars, uint reference_star_id);
double angle_to_origin (db &star);
vector<db> relevant_fov(vector<db> &lf_list, uint reference_star_id, uint reference_nn_id, database_parameters &db);
double angle2d (db &star);
void rotate2d (vector<db> &star_list, double &theta);
void z_norm (vector<db> &star_list, database_parameters &db, grid_parameters &gp);
vector<int> cells (vector<db> &star_list, grid_parameters &gp);
uint bucket_size(int g);
vector<unsigned long> bit_descriptor (vector<int> &cells, grid_parameters &gp);
void sort_nn (vector<db> &stars);
void create_ref(string &ref_db_filename, vector<db> &stars, FileStorage &fs);
void create_index(string &index_filename, vector<db> &stars, FileStorage &fs);
void create_lut(string &lut_db_filename, string &lut_nn_db_filename, vector<db> &stars, grid_parameters &gp, FileStorage &fs);
void create_bin(string &bin_db_filename, vector<db> &stars, grid_parameters &gp, FileStorage &fs);

int main (/*int argc, char **argv*/)
{// Main function.
    catalog_parameters cp; grid_parameters gp; optical_parameters op; database_parameters dp;
    load_config("config/common.yml","config/dut.yml",cp,gp,op,dp);

    vector<db> database = initialize_database(cp);
    vector<db> vstars = visible_stars(database,op);
    next_neighbor(vstars,dp);
    vector<db> rstars = reference_stars(vstars);

    calculate_descriptors(vstars,rstars,dp,gp);
    sort_nn(rstars);

    mkdir("config", S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    mkdir("config/database/", S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    string ref_db_filename =    "config/database/ref_db.bin";
    string lut_db_filename =    "config/database/lut_db.bin";
    string lut_nn_db_filename = "config/database/lut_nn_db.bin";
    string bin_db_filename =    "config/database/bin_db.bin";
    string index_filename =     "config/database/index.bin";

    FileStorage fs("config/database.yml", FileStorage::WRITE);

    fs << "ref_db_filename" << ref_db_filename;
    create_ref(ref_db_filename,rstars,fs);

    fs << "lut_db_filename" << lut_db_filename;
    fs << "lut_nn_db_filename" << lut_nn_db_filename;
    create_lut(lut_db_filename,lut_nn_db_filename,rstars,gp,fs);

    fs << "bin_db_filename" << bin_db_filename;
    create_bin(bin_db_filename,rstars,gp,fs);

    fs << "index_filename" << index_filename;
    create_index(index_filename,rstars,fs);
    fs.release();
    return 0;
}

void load_config(string filename_common, string filename_dut, catalog_parameters &cp, grid_parameters &gp, optical_parameters &op, database_parameters &dp)
{// Load configuration from file.
    double dtmp;
    Mat mtmp;
    FileStorage fs(filename_common, FileStorage::READ);
    fs["cp_hipparcos_filename"]  >> cp.hipparcos;
    fs["cp_hipparcos2_filename"] >> cp.hipparcos2;
    fs["cp_hipparcos_size"]      >> dtmp; cp.hipparcos_size = (int)dtmp;
    fs["cp_epoch"]               >> dtmp; cp.epoch = (int)dtmp;
    fs["op_resolution"]          >> mtmp; op.resolution = Point2i(mtmp);
    fs["op_pixel_size"]          >> mtmp; op.pixel_size = Point2d(mtmp);
    fs["op_sensor_size"]         >> mtmp; op.sensor_size = Point2d(mtmp);
    fs["op_focus"]               >> op.focus;
    fs["op_center"]              >> mtmp; op.center = Point2i(mtmp);
    fs["op_max_mag"]             >> op.max_mag;
    fs.release();
    FileStorage fs2(filename_dut, FileStorage::READ);
    fs2["gp_g"]                     >> dtmp; gp.g = (int)dtmp;
    fs2["gp_pattern_radius"]        >> dtmp; gp.pattern_radius = (int)dtmp;
    fs2["gp_buffer_radius"]         >> gp.buffer_radius;
    fs2["gp_confidence_factor"]     >> dtmp; gp.confidence_factor = (int)dtmp;
    fs2["gp_expercted_false_stars"] >> dtmp; gp.expected_false_stars = (int)dtmp;
    fs2["gp_minimim_match"]         >> dtmp; gp.minimum_match = (int)dtmp;
    fs2["gp_nn_error_angle"]        >> gp.error;
    fs2.release();
    dp.pattern_radius   = fov(gp.pattern_radius*op.pixel_size.y, op);
    dp.buffer_radius    = fov(gp.buffer_radius*op.pixel_size.y, op);
    dp.min_dist         = fov(norm(op.pixel_size), op);
    dp.relevance_radius = fov((norm(op.sensor_size)),op);
}

vector<db> initialize_database(catalog_parameters &cp)
{// Read information from Hipparcos and Hipparcos 2 catalogs for initializing the database.
    hip *hipparcos = new hip [cp.hipparcos_size];
    hip initial = {0,'\0',0};
    for(int i=0;i<cp.hipparcos_size;i++)
        hipparcos[i] = initial;

    double time_y = cp.epoch - 1991.25;

    // Hipparcos
    string line;
    ifstream myfile (cp.hipparcos);
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
            hip data;
            string sstr;
            int HIP      = stoi(line.substr(8,6));
            data.Proxy   = line.substr(15,1).compare(" ") ? 1:0;
            sstr = line.substr(41,5);
            if (!sstr.compare("     ")){
	        // cerr << "Warning: Empty Vmag (HIP=" << HIP << ")" << endl;
                continue;
            }
            data.Vmag    = stod(sstr);
            sstr = line.substr(47,1);
            if (sstr.compare(" "))
                data.VarFlag = stoi(sstr);
            else
                data.VarFlag = 0;
            hipparcos[HIP] = data;
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file \"" << cp.hipparcos2 << "\"." << endl;
        exit(1);
    }

    // Hipparcos 2
    vector<hip2> hipparcos2;
    ifstream myfile2 (cp.hipparcos2);
    if (myfile2.is_open()) {
        while ( getline (myfile2,line) ) {
            hip2 data;
            data.HIP   = stoi(line.substr(0,6));
            data.RArad = stod(line.substr(15,13));
            data.DErad = stod(line.substr(29,13));
            data.pmRA  = stod(line.substr(51,8));
            data.pmDE  = stod(line.substr(60,8));
            hipparcos2.push_back(data);
        }
        myfile2.close();
    }
    else {
        cout << "Unable to open file \"" << cp.hipparcos2 << "\"." << endl;
        exit(1);
    }

    // Database
    vector<db> database;
    for(uint i=0;i<hipparcos2.size();i++){
        db entry;
        entry.id         = hipparcos2[i].HIP;
        entry.mag        = hipparcos[entry.id].Vmag;
        entry.proxy_flag = hipparcos[entry.id].Proxy;
        entry.var_flag   = hipparcos[entry.id].VarFlag;
        proper_motion_correction(hipparcos2[i],time_y,&entry);
        entry.nn = -1;
        entry.angle_nn = DBL_MAX;
        database.push_back(entry);
    }
    delete [] hipparcos;
    return database;
}

vector<db> visible_stars(vector<db> &database, optical_parameters &op)
{// Extract only the visible stars from the database.
    vector<db> output;
    for(uint i=0;i<database.size();i++){
        if(database[i].mag <= op.max_mag)
            output.push_back(database[i]);
    }
    return output;
}

void next_neighbor(vector<db> &database, database_parameters &db)
{// Calculate the next neighbor pointer and distance, populating the database entries.
    for(uint i=0;i<database.size()-1;i++){
        for(uint j=i+1;j<database.size();j++){
            double angle = vangle(database[i].uv,database[j].uv);
            if(angle < db.buffer_radius){
                database[i].proxy_flag = 1;
                database[j].proxy_flag = 1;
            }
            else{
                if(angle < database[i].angle_nn){
                    database[i].nn = j;
                    database[i].angle_nn = angle;
                }
                if(angle < database[j].angle_nn){
                    database[j].nn = i;
                    database[j].angle_nn = angle;
                }
            }
        }
    }
}

vector<db> reference_stars(vector<db> &vstars)
{// Remove variable stars and stars with very close neighbors (double, triple...).
    vector<db> output;
    for(uint i=0;i<vstars.size();i++){
        if (!((vstars[i].var_flag >= 3) || vstars[i].proxy_flag))
            output.push_back(vstars[i]);
    }
    return output;
}

void calculate_descriptors (vector<db> &vstars, vector<db> &rstars, database_parameters &dp, grid_parameters &gp)
{// Populate the descriptor entries of the reference stars in the database.
    for(uint ref_star_id=0; ref_star_id<rstars.size(); ref_star_id++){
        int ref_nn_id = rstars[ref_star_id].nn;
        vector<db> rotated = rotate3d(vstars,rstars,ref_star_id);
        vector<db> cropped = relevant_fov(rotated,ref_star_id,ref_nn_id,dp);
        double theta = angle2d(rotated[rstars[ref_star_id].nn]);
        rotate2d(cropped,theta);
        z_norm(cropped,dp,gp);
        rstars[ref_star_id].angle_2d = theta;
        rstars[ref_star_id].cells_descriptor = cells(cropped,gp);
        rstars[ref_star_id].bit_descriptor = bit_descriptor(rstars[ref_star_id].cells_descriptor,gp);
    }
}

double vangle (vec<double,3> p1, vec<double,3> p2)
{// Angle between two vectors.
    return atan2(mag(cross(p1,p2)), dot(p1,p2));
}

quat<double> rot_quat (vec<double,3> v1, vec<double,3> v2)
{// Rotation quaternion between two unit vectors. V1 must be different than V2.
    double theta = vangle(v1,v2);
    double angle = cos(theta/2);
    vec<double,3> v = cross(v1,v2);
    normalize(v);
    v *= sin(theta/2);
    quat<double> output;
    output = {angle,v.a[0],v.a[1],v.a[2]};
    return output;
}

double fov(double size, optical_parameters op)
{// Field of view calculation.
    return 2*atan2(size,2*op.focus);
}

void proper_motion_correction(hip2 entry, int time_y, db *output)
{// Proper motion conversion, with output as an unit vector.
    double mu_alpha_rad = (entry.pmRA * M_PI) / (3600 * 1000 * 180 * cos(entry.DErad));
    double mu_delta_rad = (entry.pmDE * M_PI) / (3600 * 1000 * 180);
    double alpha        = entry.RArad + mu_alpha_rad * time_y;
    double delta        = entry.DErad + mu_delta_rad * time_y;
    output->uv = {cos(alpha) * cos(delta),
                  sin(alpha) * cos(delta),
                  sin(delta)};
}

vector<db> rotate3d(vector<db> visible_stars, vector<db> reference_stars, uint reference_star_id)
{// Rotate the database so that the reference star will appear on the origin.
    vec<double,3> origin = {0,0,1};
    quat<double> q = rot_quat(reference_stars[reference_star_id].uv, origin);
    for(uint i = 0; i<visible_stars.size(); i++){
        vec<double,3> p_ = visible_stars[i].uv;
        quat<double> p = {0,p_.a[0],p_.a[1],p_.a[2]};
        quat<double> rot_uv = q * p * inverse(q);
        visible_stars[i].uv = V(rot_uv);
    }
    return visible_stars;
}

double angle_to_origin (db &star)
{// Angle between the star in local frame and the coordinate system origin.
    vec<double,3> origin = {0,0,1};
    return vangle(star.uv, origin);
}

vector<db> relevant_fov(vector<db> &lf_list, uint reference_star_id, uint reference_nn_id, database_parameters &dp)
{// Remove stars which are outside of the field of view.
    vector<db> fov;
    for(uint i = 0; i<lf_list.size(); i++){
        if((i != reference_star_id)
                && (i != reference_nn_id)
                && (angle_to_origin(lf_list[i]) < dp.relevance_radius))
        { // Reference star and its next neighbor are excluded.
            fov.push_back(lf_list[i]);
        }
    }
    return fov;
}

double angle2d (db &star)
{// Returns the angle needed to rotate a given star so that it will fall in the x axis.
    return -atan2(star.uv.a[1], star.uv.a[0]);
}

void rotate2d (vector<db> &star_list, double &theta)
{// In-place rotation of a list of stars by a given angle.
    quat<double> q = rotz_quat(theta);
    for(uint i=0; i<star_list.size();i++){
        quat<double> p = {0,
                          star_list[i].uv.a[0],
                          star_list[i].uv.a[1],
                          star_list[i].uv.a[2]};
        star_list[i].uv = V(q*p*inverse(q));
    }
}

void z_norm (vector<db> &star_list, database_parameters &dp, grid_parameters &gp)
{// In-place linear transformation so that points fall within x=[0,g] and y=[0,g].
 // Since pr =! rr, out of bound values must be filtered later.
 // The z coordinate remains unchanged.
    for(uint i=0; i<star_list.size(); i++){
        double norm = star_list[i].uv.a[2] * tan(dp.pattern_radius) * 2;
        star_list[i].uv.a[0] /= norm;
        star_list[i].uv.a[1] /= norm;
        star_list[i].uv.a[0] += 0.5;
        star_list[i].uv.a[1] += 0.5;
        star_list[i].uv.a[0] *= gp.g;
        star_list[i].uv.a[1] *= gp.g;
        star_list[i].uv.a[0] = floor(star_list[i].uv.a[0]);
        star_list[i].uv.a[1] = floor(star_list[i].uv.a[1]);
    }
}

vector<int> cells (vector<db> &star_list, grid_parameters &gp)
{// Returns a list with the cells present on the descriptor.
    vector<int> cells;
    for(uint i=0; i<star_list.size(); i++){
        if((star_list[i].uv.a[0] >= 0) &&
                (star_list[i].uv.a[0] < gp.g) &&
                (star_list[i].uv.a[1] >= 0) &&
                (star_list[i].uv.a[1] < gp.g)){
            cells.push_back(gp.g*star_list[i].uv.a[1] + star_list[i].uv.a[0]);
        }
    }
    // Duplicates are removed (next 2 lines).
    sort(cells.begin(),cells.end());
    cells.erase(unique(cells.begin(),cells.end()), cells.end());
    return cells;
}

uint bucket_size(int g)
{// Calculates the number of buckets for the descriptor calculation from the grid size g; ex. ceil(g*g/64) for 64 bits.
    int g2 = g*g;
    int divisor = sizeof(long) << 3;
    return g2/divisor + (g2%divisor ? 1 : 0);
}

vector<unsigned long> bit_descriptor (vector<int> &cells, grid_parameters &gp)
{// Calculates a bit descriptor from the on cells.
    static unsigned int bit_size = sizeof(long) * 8; // Architecture dependent result
    // Since the needed bits usually exceed the architecture word lenghts, multiple 'buckets' hold the values.
    uint bsize = bucket_size(gp.g);
    unsigned long bucket[bsize];
    unsigned int i;
    for(i=0;i<bsize;i++)
        bucket[i] = 0;
    for(i=0;i<cells.size();i++){
        unsigned int n = cells[i]/bit_size;
        bucket[n] |= (unsigned long)1<<(cells[i]%bit_size);
    }
    vector<unsigned long> output(bucket, bucket + sizeof bucket / sizeof bucket[0]);
    return output;
}

void sort_nn (vector<db> &stars)
{// Sorts the database by the next neighbor angle.
    qsort(stars.data(),stars.size(),sizeof(db),[](const void *a_, const void *b_){
        db *a = (db*) a_;
        db *b = (db*) b_;
        return (int)(a->angle_nn > b->angle_nn);
    });
}

void create_ref(string &ref_db_filename, vector<db> &stars, FileStorage &fs)
{// Produces the reference database and writes it to a file. Format: {id, angle_nn, uv_x, uv_y, uv_z}
    int rows = stars.size();
    int cols = 5;
    double ref_db[rows][cols];
    for(uint i=0;i<stars.size();i++){
        ref_db[i][0] = stars[i].id;
        ref_db[i][1] = stars[i].angle_nn;
        ref_db[i][2] = stars[i].uv.a[0];
        ref_db[i][3] = stars[i].uv.a[1];
        ref_db[i][4] = stars[i].uv.a[2];
    }
    fs << "ref_db_rows" << rows;
    fs << "ref_db_cols" << cols;
    FILE *fp;
    fp = fopen(ref_db_filename.data(), "w");
    fwrite(ref_db,1,sizeof(ref_db),fp);
    fclose(fp);
}

int binSearch2(double what, double *data, double len)
{// Binary search method.
    int low = 0;
    int high = len - 1;
    int mid = 0;
    while (low <= high) {
        mid = (low + high) / 2;
        if (data[mid] > what)
            high = mid - 1;
        else if (data[mid] < what)
            low = mid + 1;
        else
            return mid;
    }
    return mid;
}

void create_index(string &index_filename, vector<db> &stars, FileStorage &fs)
{// Creates an index to speed up the lookup of elements by their nn distance.
    double last = stars[stars.size()-1].angle_nn;
    double data[stars.size()];
    for(uint i=0; i<stars.size();i++){
        data[i] = stars[i].angle_nn;
    }
    int multiplier = 1;
    while(true){
        multiplier *= 2;
        last *= 2;
        if(last > stars.size()) break;
    }
    int real_size = (int)(last+1);
    short index[real_size];
    for(int i=0; i<real_size; i++){
        index[i] = (short)binSearch2((double)i/multiplier,data,stars.size());
    }
    fs << "index_size" << real_size;
    fs << "index_multiplier" << multiplier;
    FILE *fp;
    fp = fopen(index_filename.data(), "w");
    fwrite(index,sizeof(short),sizeof(index),fp);
    fclose(fp);
}

void create_lut(string &lut_db_filename, string &lut_nn_db_filename, vector<db> &stars, grid_parameters &gp, FileStorage &fs)
{// Produces the (inverted) lookup table and writes it to a file.
    uint rows = (uint)gp.g*gp.g;
    vector<vector <short> > lut(rows, vector<short>());
    vector<vector <float> > lut_nn(rows, vector<float>());
    for(uint i=0;i<stars.size();i++){
        for(uint j=0;j<stars[i].cells_descriptor.size();j++){
            lut[ stars[i].cells_descriptor[j] ].push_back((short)i);
            lut_nn[ stars[i].cells_descriptor[j] ].push_back((float)stars[i].angle_nn);
        }
    }
    uint cols = 0;
    for(uint i=0;i<lut.size();i++){
        if(lut[i].size() > cols){
            cols = lut[i].size();
        }
    }
//    short lut_array[rows][cols];
//    for(uint i=0;i<rows;i++){
//        for(uint j=0;j<cols;j++){
//            lut_array[i][j] = -1;
//        }
//    }
    short * lut_array = new short[rows*cols];
    for(uint i=0; i<rows*cols;i++){
        lut_array[i] = -1;
    }
    float *lut_array_nn = new float[rows*cols];
    for(uint i=0; i<rows*cols;i++){
        lut_array_nn[i] = DBL_MAX;
    }
    for(uint i=0;i<rows;i++){
        //memcpy(&lut_array[i],lut[i].data(),lut[i].size()*sizeof(short));
        memcpy(&lut_array[i*cols],lut[i].data(),lut[i].size()*sizeof(short));
        memcpy(&lut_array_nn[i*cols],lut_nn[i].data(),lut_nn[i].size()*sizeof(float));
    }
    fs << "lut_db_rows" << (int)rows;
    fs << "lut_db_cols" << (int)cols;
    FILE *fp;
    fp = fopen(lut_db_filename.data(), "w");
//    fwrite(lut_array,sizeof(short),sizeof(lut_array),fp);
    fwrite(lut_array,sizeof(short),rows*cols,fp);
    fclose(fp);
    fp = fopen(lut_nn_db_filename.data(), "w");
    fwrite(lut_array_nn,sizeof(float),rows*cols,fp);
    fclose(fp);
    delete lut_array_nn;
}

void create_bin(string &bin_db_filename, vector<db> &stars, grid_parameters &gp, FileStorage &fs)
{// Produces the bit descriptor and writes it to a file.
    uint rows = stars.size();
    uint cols = bucket_size(gp.g);
    unsigned long bin_db[rows][cols];
    for(uint i=0;i<rows;i++){
        memcpy(&bin_db[i],stars[i].bit_descriptor.data(),cols*sizeof(unsigned long));
    }
    fs << "bin_db_rows" << (int)rows;
    fs << "bin_db_cols" << (int)cols;
    FILE *fp;
    fp = fopen(bin_db_filename.data(), "w");
    fwrite(bin_db,1,sizeof(bin_db),fp);
    fclose(fp);
}
