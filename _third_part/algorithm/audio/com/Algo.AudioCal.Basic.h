#pragma once
#if 1
#include"Algo.AudioCal.Com.h"
namespace Algo {
	namespace Audio {

#if 1	//shift
		template<i16 dir, class Ti, class To, class Tx>
		class Shift_c {
		public:
			Shift_c() {};
			~Shift_c() {};
		public:
			STATIC INLINE To Shift(Ti in, i16 sht) {
				if (dir == 1)
					return (To)(((Tx)in) >> sht);
				else if (dir == 1)
					return (To)(((Tx)in) << sht);
				else
					return (To)(Tx)in;
			}
		};

		template<class Ti, class To, class Tx>
		class Shift_c<1,Ti,To, Tx> {
		public:
			Shift_c() {};
			~Shift_c() {};
		public:
			STATIC INLINE To Shift(Ti in, i16 sht) {
				return (To)(((Tx)in) >> sht);
			}
		};

		template<class Ti, class To, class Tx>
		class Shift_c<-1, Ti, To, Tx> {
		public:
			Shift_c() {};
			~Shift_c() {};
		public:
			To Shift(Ti in, i16 sht) {
				return (To)(((Tx)in) << sht);
			}
		};

		template<class Ti, class To, class Tx>
		class Shift_c<0, Ti, To, Tx> {
		public:
			Shift_c() {};
			~Shift_c() {};
		public:
			STATIC INLINE To Shift(Ti in, i16 sht) {
				return (To)(Tx)in;
			}
		};
#endif





}

}


#endif