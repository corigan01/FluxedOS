#ifndef VECTOR_H_
#define VECTOR_H_

#include "../core/core.h"
#include "../VGA/VGA.h"
#include "../mem/mem.h"


#define VECTOR_MAX_SIZE 4096






template <class T>
class Vector {
    struct D {
        T Data; // Data Storage
        int PointsTo = 0; // The index this unit will point to 
        int DoesPoint = 0; // This tells if this data has been popped 
    };
    D *h;

    int VectorSize = 0;
    int EndOfVector = 0;

    public:

        Vector() {
            // make a vector of size VECTOR_MAX_SIZE (this will be the max size of a vector untill I finish paging)
            h = (D *)malloc(sizeof(D) * VECTOR_MAX_SIZE);
        }

        ~Vector() {
            // free the vector
            free(h);
        }

        void push_back(T data) {

            for (int i = 0; i < VectorSize ; i++) {
                if (h[i].DoesPoint == 0) {

                    // Assign Vector
                    h[i].PointsTo = EndOfVector;
                    h[i].DoesPoint = 1;
                    h[i].Data = data;
                    EndOfVector++;

                    //this->PrintVector();

                    return;
                }
            }

            if (VectorSize >= VECTOR_MAX_SIZE) {
                G_ERR(VGA::PRINT_STR("Vector at max size\n"));
                return;
            }

            h[VectorSize].PointsTo = EndOfVector;
            h[VectorSize].DoesPoint = 1;
            h[VectorSize].Data = data;

            EndOfVector++;   
            VectorSize++;

            //this->PrintVector();
                 
        }

        void pop_back() {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].PointsTo == (EndOfVector - 1)) {
                    

                    h[i].DoesPoint = 0;
                    h[i].Data = NULL;
                    EndOfVector--;
                    
                    return;
                }
            }
        }

        int size() {
            return EndOfVector;
        }

        void PrintVector() {
            int addit = 0;
            for (int i = 0; i < VectorSize; i++ ) {
                for (int e = 0; e < VectorSize; e++) {
                    if (h[e].PointsTo == addit && h[e].DoesPoint == true) {
                        //if (h[e].DoesPoint == false);
                        /*
                        VGA::PRINT_INT(e);
                        VGA::PRINT_STR(" --> \'");
                        VGA::PRINT_CHAR(h[e].Data);
                        VGA::PRINT_STR("\' [");
                        VGA::PRINT_INT(h[e].PointsTo);
                        VGA::PRINT_STR(", ");
                        VGA::PRINT_INT(h[e].DoesPoint);
                        VGA::PRINT_STR("] \n");
                        */
                        addit++;
                        break;
                    }
                }
            }

            ///*
            for (int i = 0; i < VectorSize; i++ ) {
                if (!h[i].DoesPoint) continue;
                VGA::PRINT_INT(i);
                VGA::PRINT_STR(" --> \'");
                VGA::PRINT_CHAR(h[i].Data);
                VGA::PRINT_STR("\' [");
                VGA::PRINT_INT(h[i].PointsTo);
                VGA::PRINT_STR(", ");
                VGA::PRINT_INT(h[i].DoesPoint);
                VGA::PRINT_STR("] \n");
            }
            //*/

            /*
            if (!h[i].DoesPoint) continue;
                VGA::PRINT_INT(i);
                VGA::PRINT_STR(" --> \'");
                VGA::PRINT_CHAR(h[i].Data);
                VGA::PRINT_STR("\' [");
                VGA::PRINT_INT(h[i].PointsTo);
                VGA::PRINT_STR(", ");
                VGA::PRINT_INT(h[i].DoesPoint);
                VGA::PRINT_STR("] \n");
            */
        }


        void pop_at(size_t s) {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].PointsTo == s) {
                    h[i].DoesPoint = 0;
                    h[i].Data = NULL;
                    EndOfVector--;
                    IncRemaining(i, -1);

                    return;
                }
            }
        }

        void insert_at(size_t s, T d) {

            // This is the easy way before we do the hard way
            // this will check if the value at the index does not point, and insert there
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].DoesPoint == false) {

                    IncRemaining(s - 1, 1);

                
                    
                    
                    h[i].DoesPoint = 1;
                    h[i].Data = d;
                    h[i].PointsTo = s;

                    EndOfVector++;

                    return;
               
                }
            }

            // This is the hard way, we now have to insert a new addr for that value
            
            
            
            IncRemaining(s - 1 , 1);
            
            h[VectorSize].DoesPoint = 1;
            h[VectorSize].PointsTo = s;
            h[VectorSize].Data = d;

            EndOfVector++;
            VectorSize++;

        }

        T& operator  [](size_t s) {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].DoesPoint == 1 && h[i].PointsTo == s) return h[i].Data;

            }

            return h[-1].Data; // we could not find that value
        }

        void operator =(Vector s) {
            this->EndOfVector = s.EndOfVector;
            this->VectorSize  = s.VectorSize;
            this.h            = s.h;
        }

       



    private:

        // This will increment the remaining vector numbers depening on what they point to
        // [ =  =  =  =  =  =] 1, 2, 3, 4, 5, 6 --> 1, 2, [3], 4, 5, 6, 7
        void IncRemaining(int adr, int inc) {
            if (adr < 0) {
                for (int i = 0; i < VectorSize; i++) {
                    h[i].PointsTo += inc;
                }
                return;
            }
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].PointsTo > adr) {
                    h[i].PointsTo += inc;
                }
            }
        }

       


};




#endif // !VECTOR_H_
