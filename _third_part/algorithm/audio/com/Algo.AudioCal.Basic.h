#pragma once
#if 1
#include"Algo.AudioCal.Com.h"
namespace Algo {
	namespace Audio {

#if 1	//shift
		template<class Ti, class To, class Tx, int N, int NN, char shiftType = (N > 0 ? 1 : (N < 0 ? -1 : 0))>
		struct RShift_t {
			static inline To Run(Ti x) {
				return (Tx)x;
			}
		};

		template<class Ti, class To, class Tx, int N, int NN>
		struct RShift_t<Ti, To, Tx, N, NN, 1> {
			static inline To Run(Ti x) {
				return  (To)(((Tx)x) >> N);
			}
		};

		template<class Ti, class To, class Tx, int N, int NN>
		struct RShift_t<Ti, To, Tx, N, NN, -1> {
			static inline To Run(Ti x) {
				return  (To)(((Tx)x) << NN);
			}
		};

		
		class RShift_c {
		public:
			RShift_c() {}
			~RShift_c() {}
		public:
			template<int N, class Ti, class To = Ti, class Tx = Ti>
			static inline To Run(Ti x) {
				return RShift_t<Ti, To, Tx, N, -N>::Run(x);
			}
		}; 
#endif





}

}


#endif