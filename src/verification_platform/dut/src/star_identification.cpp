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

#include <iostream> // DEBUG ONLY
#include "star_identification.h"

using namespace std;
using namespace cv;
using namespace std_str;

namespace st{

StarIdentification::StarIdentification()
{// Initialize the StarIdentification object.
    mode = REFERENCE;
    loadConfig();
    loadDatabase();
}

void StarIdentification::reload()
{// Reload configuration (required after changing the config structure).
    loadDatabase();
}

void StarIdentification::loadConfig()
{// Load configuration from files.
    Mat mtmp;
    double dtmp;
    FileStorage fs0("config/common.yml", FileStorage::READ);
    fs0["op_resolution"]         >> mtmp; config.optical_parameters.resolution = (Point2i)mtmp;
    fs0["op_pixel_size"]         >> mtmp; config.optical_parameters.pixel_size = (Point2d)mtmp;
    fs0["op_sensor_size"]        >> mtmp; config.optical_parameters.sensor_size = (Point2d)mtmp;
    fs0["op_focus"]              >> config.optical_parameters.focus;
    fs0["op_center"]             >> mtmp; config.optical_parameters.center = (Point2i)mtmp;
    fs0["op_max_mag"]            >> config.optical_parameters.max_mag;
    fs0.release();

    FileStorage fs1("config/database.yml", FileStorage::READ);
    fs1["ref_db_filename"]    >> config.database_parameters.ref_db_filename;
    fs1["ref_db_rows"]        >> config.database_parameters.ref_db_rows;
    fs1["ref_db_cols"]        >> config.database_parameters.ref_db_cols;
    fs1["lut_db_filename"]    >> config.database_parameters.lut_db_filename;
    fs1["lut_nn_db_filename"] >> config.database_parameters.lut_nn_db_filename;
    fs1["lut_db_rows"]        >> config.database_parameters.lut_db_rows;
    fs1["lut_db_cols"]        >> config.database_parameters.lut_db_cols;
    fs1["bin_db_filename"]    >> config.database_parameters.bin_db_filename;
    fs1["bin_db_rows"]        >> config.database_parameters.bin_db_rows;
    fs1["bin_db_cols"]        >> config.database_parameters.bin_db_cols;
    fs1["index_filename"]     >> config.database_parameters.index_filename;
    fs1["index_size"]         >> config.database_parameters.index_size;
    fs1["index_multiplier"]   >> config.database_parameters.index_multiplier;
    fs1.release();

    FileStorage fs2("config/dut.yml", FileStorage::READ);
    fs2["gp_g"]                     >> dtmp; config.grid_parameters.g = (int)dtmp;
    fs2["gp_pattern_radius"]        >> dtmp; config.grid_parameters.pattern_radius = (int)dtmp;
    fs2["gp_buffer_radius"]         >> config.grid_parameters.buffer_radius;
    fs2["gp_confidence_factor"]     >> dtmp; config.grid_parameters.confidence_factor = (int)dtmp;
    fs2["gp_expected_false_stars"]  >> dtmp; config.grid_parameters.expected_false_stars = (int)dtmp;
    fs2["gp_minimum_match"]         >> dtmp; config.grid_parameters.minimum_match = (int)dtmp;
    fs2["gp_nn_error_angle"]        >> config.grid_parameters.error;
    fs2["gp_alg_variation"]         >> dtmp; mode = (Stmode)dtmp;
    fs2.release();
}

void StarIdentification::loadDatabase()
{// Load database from file.
    ref_cat = new double*[config.database_parameters.ref_db_rows];
    ref_cat[0] = new double[config.database_parameters.ref_db_rows * config.database_parameters.ref_db_cols];
    for(int i = 1; i<config.database_parameters.ref_db_rows; i++)
        ref_cat[i] = ref_cat[i-1] + config.database_parameters.ref_db_cols;

    bin_cat = new unsigned long long*[config.database_parameters.bin_db_rows];
    bin_cat[0] = new unsigned long long[config.database_parameters.bin_db_rows * config.database_parameters.bin_db_cols];
    for(int i = 1; i<config.database_parameters.bin_db_rows; i++)
        bin_cat[i] = bin_cat[i-1] + config.database_parameters.bin_db_cols;

    lut_cat = new short*[config.database_parameters.lut_db_rows];
    lut_cat[0] = new short[config.database_parameters.lut_db_rows * config.database_parameters.lut_db_cols];
    for(int i = 1; i<config.database_parameters.lut_db_rows; i++)
        lut_cat[i] = lut_cat[i-1] + config.database_parameters.lut_db_cols;

    lut_nn_cat = new float*[config.database_parameters.lut_db_rows];
    lut_nn_cat[0] = new float[config.database_parameters.lut_db_rows * config.database_parameters.lut_db_cols];
    for(int i = 1; i<config.database_parameters.lut_db_rows; i++)
        lut_nn_cat[i] = lut_nn_cat[i-1] + config.database_parameters.lut_db_cols;

    index = new short[config.database_parameters.index_size];

    initialize_catalog(ref_cat[0],
            config.database_parameters.ref_db_rows*config.database_parameters.ref_db_cols*sizeof(double),
            config.database_parameters.ref_db_filename); // Line form: {hip, nn-angle, uv_x, uv_y, uv_z}
    initialize_catalog(bin_cat[0],
            config.database_parameters.bin_db_rows*config.database_parameters.bin_db_cols*sizeof(unsigned long long),
            config.database_parameters.bin_db_filename);
    initialize_catalog(lut_cat[0],
            config.database_parameters.lut_db_rows*config.database_parameters.lut_db_cols*sizeof(short),
            config.database_parameters.lut_db_filename);
    initialize_catalog(lut_nn_cat[0],
            config.database_parameters.lut_db_rows*config.database_parameters.lut_db_cols*sizeof(float),
            config.database_parameters.lut_nn_db_filename);
    initialize_catalog(index,
            config.database_parameters.index_size*sizeof(short),
            config.database_parameters.index_filename);
}

StarIdentification::~StarIdentification()
{// Destructor.
    delete ref_cat[0];
    delete bin_cat[0];
    delete lut_cat[0];
    delete ref_cat;
    delete bin_cat;
    delete lut_cat;
}

Sky StarIdentification::identifyStars(Sky csky)
{// Star identification procedure.
    vector<Candidate> candidates;
    classifier(candidates,csky.stars);
    vector<vector<Candidate> > clustered;
    double fov_2 = fov(cblas_dnrm2(2, &config.optical_parameters.sensor_size.x, 1), config.optical_parameters.focus);
    cluster(clustered, candidates, fov_2);
    vector<Candidate>* verified = verify(clustered);
    if(verified) filter(*verified,fov_2/2);
    vector<Star> identified = format_output(verified);
    csky.stars = identified;
    return csky;
}

void StarIdentification::initialize_catalog(void *ptr, size_t size, string filename)
{// Initializes the catalog array from filename.
    FILE *fp;
    fp = fopen(filename.data(), "r");
    fread(ptr, 1, size, fp);
    fclose(fp);
}

void StarIdentification::translate_rotate (std::vector<cv::Point2d> &translated, double &nn_dist, vector<Star> stars, uint ref)
{// Translates the points so that the reference sits in the north pole, and rotate it so that the closest neighbor will lie on the x axys.
    nn_dist = numeric_limits<double>::max(); // euclidean distance for the nearest neighbor
    cv::Point2d closest_neighbor;
    closest_neighbor.x = 0;
    closest_neighbor.y = 0;
    int closest_neighbor_id = -1;
    for(uint i=0;i<stars.size();i++){
        if(i!=ref){
            // Translation using ref as the origin:
            cv::Point2d point;
            point.x = stars[i].centroid.x - stars[ref].centroid.x;
            point.y = stars[i].centroid.y - stars[ref].centroid.y;
            double dist = cblas_dnrm2(2,&point.x,1); // Returns the euclidean norm of a vector
            if(dist<config.grid_parameters.pattern_radius){
                translated.push_back(point);
                // Determine the closest neighbor for rotation:
                if(dist < nn_dist && dist > config.grid_parameters.buffer_radius){
                    closest_neighbor = point;
                    closest_neighbor_id = translated.size()-1;
                    nn_dist = dist;
                }
            }
        }
    }
    if(closest_neighbor.x!=0.0 && closest_neighbor.y!=0.0){
        // Rotation with respect to (minus) theta:
        double theta = atan2(closest_neighbor.y,closest_neighbor.x);
        cblas_drot(translated.size(),&translated.data()->x,2,&translated.data()->y,2,cos(theta),sin(theta));
        translated.erase(translated.begin()+closest_neighbor_id);
    }
}

int StarIdentification::normalized_coordinate(double pixel_coordinate)
{// Normalizes the pixel coordinate (x or y), originally between [-pattern_radius,pattern-radius] to a new integer value between [0,g]
    return (int)config.grid_parameters.g*(pixel_coordinate + config.grid_parameters.pattern_radius)/(2*config.grid_parameters.pattern_radius);
}

int StarIdentification::cell(double x, double y)
{// Determines in which cell the point (x,y) is situated;
    int i = normalized_coordinate(x);
    int j = normalized_coordinate(y);
    return j*config.grid_parameters.g+i;
}

void StarIdentification::bit_descriptor (unsigned long *bucket,
                                         unsigned int bucket_size,
                                         std::vector<cv::Point2d> &translated)
{// Calculates a bit descriptor (bucket) from the translated/rotated points.
    static unsigned int bit_size = sizeof(long) * 8;
    unsigned int i;
    for(i=0;i<bucket_size;i++)
        bucket[i] = 0;
    for(i=0;i<translated.size();i++){
        int current_cell = cell(translated.at(i).x,translated.at(i).y);
        unsigned int n = current_cell/bit_size;
        bucket[n] |= (unsigned long)1<<(current_cell%bit_size);
    }
}

void StarIdentification::descriptor(vector<int> &desc, vector<cv::Point2d> &translated)
{// Calculates the descriptor from the translated/rotated points.
    unsigned int i;
    unsigned int size = config.grid_parameters.g * config.grid_parameters.g;
    int desc_array[size];
    for(i=0;i<size;i++)
        desc_array[i] = 0;
    for(i=0;i<translated.size();i++){
        desc_array[cell(translated.at(i).x,translated.at(i).y)]++;
    }
    for(i=0;i<size;i++)
        if(desc_array[i] > 0)
            desc.push_back(i);
}

void StarIdentification::pixel_to_unit_vector(cv::Point3d *unit_vector, cv::Point2d star)
{// Transforms a 2D point in the camera frame into a 3D unit vector.
    unit_vector->x = config.optical_parameters.pixel_size.x*(star.x - config.optical_parameters.center.x);
    unit_vector->y = config.optical_parameters.pixel_size.y*(star.y - config.optical_parameters.center.y);
    unit_vector->z = config.optical_parameters.focus;
    double norm = cblas_dnrm2(3,&unit_vector->x,1);
    unit_vector->x/=norm;
    unit_vector->y/=norm;
    unit_vector->z/=norm;
}

int StarIdentification::array_intersection(int* array0, int size0, int* array1, int size1, int max_element)
{// Determines the number of identical elements between two arrays.
    int seen[max_element];
    int count = 0;
    int i;
    for(i=0;i<max_element;i++)
        seen[i] = 0;
    for(i=0;i<size0;i++)
        seen[array0[i]]++;
    for(i=0;i<size1;i++)
        if(seen[array1[i]] > 0){
            count++;
            seen[array1[i]] = 0; //remove duplicates
        }
    return count;
}


int StarIdentification::classify(int *max, int *desc_size, vector<cv::Point2d> &translated)
{// Builds the descriptor and classifies the star based on it.
    int output=-1;
    int v[config.database_parameters.ref_db_rows];
    *max = 0;
    vector<int> desc;
    descriptor(desc,translated); // builds the descriptor
    *desc_size = desc.size();
    for(int i=0;i<config.database_parameters.ref_db_rows;i++)
        v[i] = 0;
    for(uint i=0;i<desc.size();i++){
        for(int j=0;j<config.database_parameters.lut_db_cols;j++){
            int position = lut_cat[ desc.at(i) ][j];
            if(position < 0)
                break;
            int value = ++v[position];
            if(value > *max){
                output = position;
                *max = value;
            }
        }
    }
    return output;
}

int StarIdentification::classifyK(int *max, int *desc_size, vector<cv::Point2d> &translated, double nn_angle)
{// Builds the descriptor and classifies the star based on it.
    int output=-1;
    int v[config.database_parameters.ref_db_rows];
    vector<int> desc;
    descriptor(desc,translated); // builds the descriptor
    *desc_size = desc.size();
    //*max = - (*desc_size);
    *max = 0;
    for(int i=0;i<config.database_parameters.ref_db_rows;i++)
        v[i] = *max;
    for(uint i=0;i<desc.size();i++){
        double first = binSearch2(nn_angle - config.grid_parameters.error,
                                  lut_nn_cat[desc[i]],
                                  config.database_parameters.lut_db_cols);
        for(int j=first; j<config.database_parameters.lut_db_cols; j++){
            int position = lut_cat[ desc[i] ][j];
            double nn_dist = lut_nn_cat[ desc[i] ][j];
            if((position < 0) || (nn_dist > (nn_angle + config.grid_parameters.error)))
                break;
            //int value = v[position] += 2;
            int value = ++v[position];
            if(value > *max){
                output = position;
                *max = value;
            }
        }
    }
    return output;
}

int StarIdentification::binSearch(double what)
{// Binary search for the classify_binary method.
    int len = config.database_parameters.ref_db_rows;
    int low = 0;
    int high = len - 1;
    int mid = 0;
    while (low <= high) {
        mid = (low + high) / 2;
        if (ref_cat[mid][1] > what)
            high = mid - 1;
        else if (ref_cat[mid][1] < what)
            low = mid + 1;
        else
            return mid;
    }
    return mid;
}

int StarIdentification::binSearch2(float what, float *data, int len)
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

int StarIdentification::classify_binary(int *max, int *desc_size, vector<cv::Point2d> &translated, double nn_angle)
{// Builds the descriptor and classifies the star based on it.
    static unsigned int bucket_size = config.database_parameters.bin_db_cols * (8 / sizeof(long)); // Architecture dependent hack
    unsigned long bucket0[bucket_size]; // Generate the bit descriptor from the list descriptor
    bit_descriptor(bucket0,bucket_size,translated);

    *desc_size = 0;
    for(unsigned int j=0;j<bucket_size;j++)
        *desc_size += __builtin_popcountl(bucket0[j]); // Compiler built-in function

    int output=-1;
    *max = 0;
    int begin = binSearch(nn_angle - config.grid_parameters.error);
    for(int i=begin;i<config.database_parameters.bin_db_rows;i++){
        if(ref_cat[i][1] > (nn_angle + config.grid_parameters.error))
            break;
        int score = 0;
        for(unsigned int j=0;j<bucket_size;j++){
            unsigned long* cat = (unsigned long *)&bin_cat[i][0];
            score += __builtin_popcountl(bucket0[j]&cat[j]); // Compiler built-in function
        }
        if(score > *max){
            output = i;
            *max = score;
        }
    }
    return output;
}

int StarIdentification::classify_binary_index(int *max, int *desc_size, vector<cv::Point2d> &translated, double nn_angle)
{// Builds the descriptor and classifies the star based on it.
    static unsigned int bucket_size = config.database_parameters.bin_db_cols * (8 / sizeof(long)); // Architecture dependent hack
    unsigned long bucket0[bucket_size]; // Generate the bit descriptor from the list descriptor
    bit_descriptor(bucket0,bucket_size,translated);

    *desc_size = 0;
    for(unsigned int j=0;j<bucket_size;j++)
        *desc_size += __builtin_popcountl(bucket0[j]); // Compiler built-in function

    int output=-1;
    *max = 0;

    int nn_begin = (int)((nn_angle - config.grid_parameters.error) * config.database_parameters.index_multiplier);
    int nn_end   = (int)((nn_angle + config.grid_parameters.error) * config.database_parameters.index_multiplier);
    nn_begin = (nn_begin < 0) ? 0 : nn_begin;
    nn_end   = (nn_end   < 0) ? 0 : nn_end;
    nn_begin = (nn_begin > (config.database_parameters.index_size - 1)) ? (config.database_parameters.index_size - 1) : nn_begin;
    nn_end   = (nn_end   > (config.database_parameters.index_size - 1)) ? (config.database_parameters.index_size - 1) : nn_end;
    int begin = index[nn_begin];
    int end   = index[nn_end];

    for(int i=begin;i<end;i++){
        int score = 0;
        for(unsigned int j=0;j<bucket_size;j++){
            unsigned long* cat = (unsigned long *)&bin_cat[i][0];
            score += __builtin_popcountl(bucket0[j]&cat[j]); // Compiler built-in function
        }
        if(score > *max){
            output = i;
            *max = score;
        }
    }
    return output;
}

int StarIdentification::classify_binary_ref(int *max, int *desc_size, vector<cv::Point2d> &translated)
{// Builds the descriptor and classifies the star based on it.
    static unsigned int bucket_size = config.database_parameters.bin_db_cols * (8 / sizeof(long)); // Architecture dependent hack
    unsigned long bucket0[bucket_size]; // Generate the bit descriptor from the list descriptor
    bit_descriptor(bucket0,bucket_size,translated);

    *desc_size = 0;
    for(unsigned int j=0;j<bucket_size;j++)
        *desc_size += __builtin_popcountl(bucket0[j]); // Compiler built-in function

    int output=-1;
    *max = 0;
    for(int i=0;i<config.database_parameters.bin_db_rows;i++){

        int score = 0;
        for(unsigned int j=0;j<bucket_size;j++){
            unsigned long* cat = (unsigned long *)&bin_cat[i][0];
            score += __builtin_popcountl(bucket0[j]&cat[j]); // Compiler built-in function
        }
        if(score > *max){
            output = i;
            *max = score;
        }
    }
//    cout << output << "\t" << *max << endl;
    return output;
}

void StarIdentification::classifier(vector<Candidate> &candidates, vector<Star> stars)
{// Classifies all the stars in the image. Returns the star ids(catalog) and relative(camera center) coordinates.
    qsort(stars.data(),stars.size(),sizeof(stars[0]),[](const void *a_, const void *b_){ // Sort the centers of mass of the stars by their brightness.
        const Star *a = (const Star*) a_;
        const Star *b = (const Star*) b_;
        int value = a->mag > b->mag;
        return (int)(value);});

    uint nsize = (uint)config.grid_parameters.confidence_factor < stars.size() ? config.grid_parameters.confidence_factor : stars.size();
    for(uint i=0;i<nsize;i++){
        vector<cv::Point2d> translated;
        double nn_dist;
        translate_rotate(translated,nn_dist,stars,i);
        double nn_angle = atan2(nn_dist*config.optical_parameters.pixel_size.x, config.optical_parameters.focus);
        int max, desc_size;
        Candidate cd;

        switch (mode) {
        case REFERENCE:
            cd.pos_id = classify(&max,&desc_size,translated);
            break;
        case REFERENCE_BIN:
            cd.pos_id = classify_binary_ref(&max,&desc_size,translated);
            break;
        case BINARY:
            cd.pos_id = classify_binary(&max,&desc_size,translated,nn_angle);
            break;
        case BINARY_INDEX:
            cd.pos_id = classify_binary_index(&max,&desc_size,translated,nn_angle);
            break;
        case K:
            cd.pos_id = classifyK(&max,&desc_size,translated,nn_angle);
            break;
        default:
            cerr << "Invalid mode." << endl;
            exit(0xFF);
            break;
        }

        cd.mag      = stars[i].mag;
        cd.centroid = stars[i].centroid;
        if((cd.pos_id >= 0)){
//                && (max >= config.grid_parameters.minimum_match)
//                && (max >= ((int)desc_size - config.grid_parameters.expected_false_stars))){
            pixel_to_unit_vector(&cd.unit_vector,stars[i].centroid);
            candidates.push_back(cd);
        }
    }
}

cv::Point3d StarIdentification::cross(cv::Point3d a, cv::Point3d b)
{// Computes the cross product between the two 3-dimensional points a and b.
    cv::Point3d result;
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    return result;
}

double StarIdentification::vector_angle(cv::Point3d a, cv::Point3d b)
{// Returns the angle between two vectors.
    cv::Point3d cross_product = cross(a,b);
    double norm = cblas_dnrm2(3,&cross_product.x,1);
    double dot_product = cblas_ddot(3,&a.x,1,&b.x,1);
    return atan2(norm,dot_product);
}

void StarIdentification::cluster(vector<vector<Candidate> > &output, vector<Candidate> &candidates, double v_fov)
{// Cluster together the stars within the same Field of View.
    vector<Candidate> rejected = candidates;
    while(rejected.size()){
        vector<Candidate> accepted;
        vector<Candidate> tmp = rejected;
        rejected.clear();
        accepted.push_back(tmp.at(0)); // Position 0 is the reference
        cv::Point3d* reference = (cv::Point3d*)&ref_cat[tmp.at(0).pos_id][2];
        for(uint i=1;i<tmp.size();i++){
            cv::Point3d* actual    = (cv::Point3d*)&ref_cat[tmp.at(i).pos_id][2];
            if(vector_angle(*reference,*actual) < v_fov)
                accepted.push_back(tmp.at(i));
            else
                rejected.push_back(tmp.at(i));
        }
        output.push_back(accepted);
    }
}

void StarIdentification::filter(vector<Candidate> & cluster, double fov)
{// Second pass filter for the actual (smaller) FOV.
    Point3d center = {0.0,0.0,0.0};
    uint i;
    for(i=0; i<cluster.size(); i++){
        cv::Point3d* value = (cv::Point3d*)&ref_cat[cluster.at(i).pos_id][2];
        center.x += value->x;
        center.y += value->y;
        center.z += value->z;
    }
    center.x /= i;
    center.y /= i;
    center.z /= i;
    for(i=0; i<cluster.size(); i++){
        cv::Point3d* value = (cv::Point3d*)&ref_cat[cluster.at(i).pos_id][2];
        if(vector_angle(*value, center) > fov){
            cluster.erase(cluster.begin() + i);
        }
    }
}

vector<Candidate>* StarIdentification::verify (vector<vector<Candidate> > &clustered)
{// Verifies if the first and second largest clusters are not of the same size. Outputs a NULL pointer otherwise.
    vector<Candidate>* output = NULL;
    // Sort a vector of candidates the size of their members (largest to lowest):
    qsort(clustered.data(),clustered.size(),sizeof(vector<Candidate>),[](const void *a_, const void *b_){
        vector<Candidate> *a = (vector<Candidate>*) a_;
        vector<Candidate> *b = (vector<Candidate>*) b_;
        return (int)(a->size() < b->size());
    });
    if(clustered.size() > 0){
        output = &clustered.at(0);
    }
    if(clustered.size() > 1 && (clustered.at(0).size() == clustered.at(1).size())){
        output = NULL;
    }
    return output;
}

double StarIdentification::fov(double size, double focus)
{// Field of View angle calculation.
    return 2*atan2(size,focus*2);
}

Star StarIdentification::to_star(Candidate cd)
{// Creates the structure sky used as the output of the Grid Algorithm.
    Star output;
    output.id            = ref_cat[cd.pos_id][0];
    output.mag           = cd.mag;
    output.centroid      = cd.centroid;
    output.uv_camera     = cd.unit_vector;
    output.uv_inertial.x = ref_cat[cd.pos_id][2];
    output.uv_inertial.y = ref_cat[cd.pos_id][3];
    output.uv_inertial.z = ref_cat[cd.pos_id][4];
    return output;
}

vector<Star> StarIdentification::format_output(vector<Candidate> *verified)
{// Formats all the identified stars into the star_id structure and outputs them as a vector.
    vector<Star> output;
    if(verified != NULL){
        for(uint i=0; i<verified->size(); i++){
            output.push_back(to_star(verified->at(i)));
        }
    }
    return output;
}

}
