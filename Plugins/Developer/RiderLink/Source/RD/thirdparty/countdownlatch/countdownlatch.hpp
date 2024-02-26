#ifndef __COUNTDOWNLATCH_NIPUN__
#define __COUNTDOWNLATCH_NIPUN__

#include <cinttypes>
#include <cstdint>
#include <mutex>
#include <condition_variable>


namespace clatch 
{
    class countdownlatch {
    public:
        /*! Constructor
          \param count, the value the countdownlatch object should be initialized with
        */
        explicit countdownlatch(uint32_t count);


        /*!
            await causes the caller to wait until the latch is counted down to zero, 
            if wait time nanosecs is not zero, then maximum wait is for nanosec nanoseconds 
          \param nanosecs is waittime in nanoseconds, by default it is zero which specifies 
           indefinite wait  
        */
        void await(uint64_t nanosecs=0);

        /*!
          Countdown decrements the count of the latch, signalling all waiting thread if the 
          count reaches zero.
         */
        void count_down();

        /*!
          get_count returns the current count  
         */
        uint32_t get_count(); 

    private:
        std::condition_variable cv;
        std::mutex lock;
        uint32_t count;
        
        // deleted constructors/assignmenet operators
        countdownlatch() = delete;
        countdownlatch(const countdownlatch& other) = delete;
        countdownlatch& operator=(const countdownlatch& opther) = delete;
    };
}

#endif
