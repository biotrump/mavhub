// opencv based kalman filter

#ifndef _FILTER_KALMANCV_H_
#define _FILTER_KALMANCV_H_

#include "logger.h"
#include "protocollayer.h"

#include "opencv/cv.h" // opencv headers

// using namespace cv;

namespace mavhub {
	/// Kalman filter class: hover (altitude)
  class Kalman_CV {
  public:
		/// Constructor
		Kalman_CV();
		virtual ~Kalman_CV();

		/// initialize kalman
		virtual void init();
		/// evaluate kalman: predict + correct
		virtual void eval();
		/// update transition matrix
		virtual void update_F_dt(double dt);

		/// static cv matrix print function
		static inline void cvPrintMat(CvMat* M, int rows, int cols, char* name) {
			int i,j;
			printf("%s = [\n", name);
			for(i=0; i < rows; i++) {
				for(j=0; j< cols; j++) {
					printf("%f ", cvmGet(M, i, j));
				}
				printf("\n");
			}
			printf("]\n");
		}
		/// kalman Transition matrix getter
		virtual CvMat* getTransMat();

		/// kalman measurement getter
		inline CvMat* getMeas() {
			return meas;
		}

		/// kalman measurement setter
		inline void setMeasAt(int row, int col, double val) {
			// XXX: check bound violation
			cvmSet(meas, row, col, val);
		}

		// protected:
  private:
		CvKalman* cvkal;
		CvMat* meas;
  };
}

#endif