#include <Eigen/Dense>
#include <iostream>
#include <algorithm>
#include <random>
#include <any>
#include <cmath>
#include <string>
#include <map>

int calculate_splits(int split_no){
    int catcher = 0; 
    for (int i = 1; i <= split_no; i++){
        if (i == 1 || i == 2){
            catcher += i;
        }
        else{
            catcher += std::pow(2,i-2+1);
        }
    }

    return catcher;
}


void print_left_n_right(std::vector<int> left, std::vector<int> right, std::string ky){
    std::string l = "{";
    std::string r = "{";

    for (auto L: left){
        l+=std::to_string(L) + ",";
    } 

    for (auto R: right){
        r+=std::to_string(R) + ","; 
    }

    l += "}";
    r += "}";

    

    //std::cout << ky + "L is: " << l << std::endl;
    //std::cout << ky + "R is: " << r << "\n\n" << std::endl;
}


int main(){
    std::vector<int> feature1 = {6,9,3,20,14,17,22,7,2,12};
    std::vector<int> feature2 = {12,16,19,13,20,14,17,22,25,26};
    std::map<std::string, std::vector<int>> vals;
    std::vector<int> target_set = feature1;
    std::random_device rand_dev;
    std::mt19937 gen(rand_dev());
    std::string base;

    float anomaly_threshold = 0.75;
    float split_depth_target = 5.f;
    int number_of_splits_target = calculate_splits(split_depth_target);
    int itter;
    int max;
    int min;
    
    //std::cout << "number_of_splits_target is: " << number_of_splits_target << std::endl;    

    //FOR EACH FEATURE 
    for (int i = 1; i < 2; i++){
        
        if (i == 1){
           int cur_feature = 1; 
           std::vector<int> target_set = feature1;
        }

        else{
           int cur_feature = 2; 
           std::vector<int> target_set = feature2;
        }

        int split_count = 0;
        int split_depth = 0;
        while (split_count < number_of_splits_target){
            //CALCULATE MIN AND MAX OF TARGET VECTOR.
            for (int itt = 0; itt < target_set.size(); itt++){
                
                if (target_set.size() > 0){
                    min = target_set[0];
                    max = target_set[target_set.size()-1];
                }

                int val = target_set[itt];

                if (val < min) {
                    min = val;
                }

                if (val > max) {
                    max = val - 1;
                }

            }


            //PICK A VALUE BETWEEN MAX AND MIN.
            std::uniform_int_distribution<> distribution(min,max);
            float choice = distribution(gen) + 0.5;
            //std::cout << "random choice is: " << choice << std::endl;


            //SPLIT THE SET AT THE CHOSEN VALUE.
            std::vector<int> left;
            std::vector<int> right;

            for (int itt = 0; itt < target_set.size(); itt++){
                int val = target_set[itt];
                if (val < choice) {
                    left.push_back(val);
                }

                else if (val > choice) {
                    right.push_back(val);
                }

            }
            
            
            //UPDATE SPLIT COUNT
            split_count += 1;

            //STORE RESULTS OF THE SPLIT

            if (vals.size() > 0){ //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                std::string full = vals.rbegin()->first; 
                //std::cout << "Retreived key is: " << full << std::endl;
                int num = std::stoi(full.substr(0,1)); 
                std::string check = std::string(num, 'R'); 
                if (full.substr(1) == check){ 
                    split_depth += 1;
                    //std::cout << "changed base" << std::endl;
                    base = std::to_string(split_depth) + std::string(num, 'L');
                } 

            } else {
                split_depth += 1;
                base = std::to_string(split_depth);       
            }

            vals[base + "L"] = left;
            vals[base + "R"] = right;

            print_left_n_right(left,right,base);

            //UPDATE THE TARGET SET
            if (split_count == 1){ 
                target_set = left;
            } else { //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                
                std::string key = vals.rbegin()->first; //2LR
                int num = std::stoi(key.substr(0,1)); //2
                std::string check = std::string(num, 'R'); //RR 
           
                if (key.substr(1) == check){ //RR = RR.
                    
                    std::string key2find = std::to_string(num) + std::string(num, 'L');//if we reach 2RR set target = 2LL
                    //std::cout << "key2find = " << key2find;
                    std::map<std::string, std::vector<int>>::iterator pre_targ_set = vals.find(key2find);
                    target_set = pre_targ_set->second;//set to 1R (prev_right).
                    base = pre_targ_set->first;

                } else {//3LLR -> 3LRL -> 3LRR -> 3 
                    if (key[key.length()-1] == 'R') {
                        int search_L = 0;
                        for (auto c = key.rbegin(); c != key.rend(); ++c){
                            if (*c == 'L'){
                                //std::cout << "key was: " << key << std::endl; 
                                key[key.length() - 1 - search_L] = 'R';//4RLRR
                                int repeaters = key.substr(key.length() - search_L,key.length() - 1).length();
                                base = key.substr(0,key.length() - search_L) + std::string(repeaters - 1, 'L');
                                //std::cout << "base = " << base << std::endl;
                                std::map<std::string, std::vector<int>>::iterator pre_target = vals.find(std::to_string(num-1)+base.substr(1)); //3LRR  
                                target_set = pre_target->second;
                                //std::cout << "asked to find: " << pre_target->first << std::endl;
                                break;
                            } 
                            search_L += 1;        
                        }
                    }
                }


            } 

        }
    
    }

    std::map<int,float> anomaly_scores;
    for (std::map<std::string, std::vector<int>>::reverse_iterator x = vals.rbegin(); x != vals.rend(); ++x){
            
            if (x->second.size() == 1){                         
                anomaly_scores[x->second[0]] = std::stof(x->first.substr(0,1))  / split_depth_target;
            }          

    }

    for (auto ch : feature1){
        if (anomaly_scores.find(ch) == anomaly_scores.end()){
            anomaly_scores[ch] = 1;
        }
    }



    std::string anomaly_status;
    for (std::map<int,float>::iterator s = anomaly_scores.begin(); s != anomaly_scores.end(); ++s){
        if (s->second < 0.5f){
            anomaly_status = "anomaly";
        } else {
            anomaly_status = "non anomaly";
        }   
        std::cout << "anomaly score for " << s->first << " = " << s->second << " anomaly status = " << anomaly_status << std::endl; 
    }


    return 0;
}




/*//3LLR -> 3LRL -> 3LRR -> 3RLL -> 3RLR -> 3RRL -> 3RRR  

        |
    |       |
  |   |   |   |

 | | | | | | | |

*/