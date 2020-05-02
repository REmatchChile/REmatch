#ifndef STRUCTS__VECTOR_HPP
#define STRUCTS__VECTOR_HPP

#include <cstring>
#include <iostream>

namespace rematch {

    template <typename T>
    class vector {
        public:
            typedef T               value_type;
            typedef T &             reference;
            typedef const T &       const_reference;
            typedef T *             pointer;
			typedef const T *       const_pointer;
            typedef unsigned int    size_type;
            typedef T *             iterator;

            vector() noexcept {
                arr = new T[reserve_sz];
            }

            explicit vector(size_type n) {
                size_type i;
                reserve_sz = n << 2;
                arr = new T[reserve_sz];
                for(i=0; i < n; ++i) {
                    arr[i] = T();
                }
                vec_sz = n;
            }

            ~vector() {
                delete [] arr;
            }

            // Iterators
            iterator begin() noexcept {return arr;}
            iterator end() noexcept {return arr + vec_sz;}

            // Sizes
            bool empty() const noexcept {return vec_sz == 0;}
            size_type size() const noexcept {return vec_sz;}
            size_type capacity() const noexcept {return reserve_sz;}
            void reserve(size_type newsize) {
                if(newsize > reserve_sz) {
                    reserve_sz = newsize;
                    reallocate();
                }
            }

            // Access
            reference operator[] (size_type idx) {return arr[idx];}
            const_reference operator[] (size_type idx) const {return arr[idx];}
            reference front() {return arr[0];}
            const_reference front() const {return arr[0];}
            reference back() {return arr[vec_sz - 1];}
            const_reference back() const {return arr[vec_sz - 1];}

            void push_back(const T &val) {
                // if (vec_sz == reserve_sz) {
                //     reserve_sz <<= 2;
                //     reallocate();
                // }
                arr[vec_sz] = val;
                ++vec_sz;
            }

            void clear() noexcept {vec_sz = 0;} // TODO: Need to delete objects later?

            void swap(vector<T> &rhs) {
                size_type tvec_sz = vec_sz,
                          treserve_sz = reserve_sz;
                T *tarr = arr;

                vec_sz = rhs.vec_sz;
                reserve_sz = rhs.reserve_sz;
                arr = rhs.arr;

                rhs.vec_sz = tvec_sz;
                rhs.reserve_sz = treserve_sz;
                rhs.arr = tarr;
            }

        
        private:
            size_type reserve_sz = 4;
            size_type vec_sz = 0;
            T *arr;

            void reallocate() {
                T* tarr = new T[reserve_sz];
                memcpy(tarr, arr, vec_sz *sizeof(T));
                delete [] arr;
                arr = tarr;
            }

    }; // class vector

} // namespace structs



#endif // STRUCTS__VECTOR_HPP