#include "monitor.h"
#include "WriteOutput.h"
#include "helper.h"
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

/* START OF STRUCTS AND ENUMS */

enum connectorType
{
    NARROW_BRIDGE,
    FERRY,
    CROSSROAD
};

// Direction struct
typedef struct
{
    int from;
    int to;
} Direction;

/* END OF STRUCTS AND ENUMS */

/* START OF CLASSES */

class NarrowBridge : public Monitor
{
private:
    Condition cvDirectionZero, cvDirectionOne, cvTimerZero, cvTimerOne, cvCarPassingZero, cvCarPassingOne, cvOrderZero, cvOrderOne, cvPassDelay, cvTravelDelay;
    int active_lane = 0, active_cars = 0;
    bool carHasPassed = false, last_car_passing_zero = false, last_car_passing_one = false;
    queue<int> car_queue_zero, car_queue_one;

public:
    int id, travel_time, max_wait_time;

    NarrowBridge(int id, int travel_time, int max_wait_time) : cvDirectionZero(this), cvDirectionOne(this), cvTimerZero(this), cvTimerOne(this), cvCarPassingZero(this), cvCarPassingOne(this), cvOrderZero(this), cvOrderOne(this), cvPassDelay(this), cvTravelDelay(this)
    {
        this->id = id;
        this->travel_time = travel_time;
        this->max_wait_time = max_wait_time;
    }

    void pass_narrow_bridge(Direction *direction, int carId)
    {
        __synchronized__;
        WriteOutput(carId, 'N', this->id, ARRIVE);

        // drive into the lane queue
        if (direction->from == 0)
            car_queue_zero.push(carId);
        else
            car_queue_one.push(carId);

    beginning:
        // if currently active lane is not my direction
        if (active_lane != direction->from)
        {
            if (direction->from == 0)
            {
                // The active lane is empty, no need to start timer, just change the lane
                if (car_queue_one.empty() && active_cars == 0)
                {
                    // restart carHasPassed

                    active_lane = 0;
                    cvDirectionZero.notifyAll();
                    goto beginning;
                    // DO SOMETHING
                }
                // First waiting car starts the timer
                else if (car_queue_zero.front() == carId)
                {
                    struct timeval tv;
                    struct timespec ts;
                    gettimeofday(&tv, NULL);
                    ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                    ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                    if (ts.tv_nsec >= 1000000000UL)
                    {
                        ts.tv_nsec -= 1000000000;
                        ts.tv_sec += 1;
                    }

                    int rv = cvTimerZero.timedwait(&ts);
                    if (rv == ETIMEDOUT)
                    {
                        // restart carHasPassed
                        active_lane = 0;
                        cvDirectionZero.notifyAll();
                        goto beginning;
                        // DO SOMETHING AFTER TIMEOUT
                    }

                    goto beginning;
                }
                // There's nothing to do, wait
                else
                {
                    cvDirectionZero.wait();
                    goto beginning;
                }
            }
            else if (direction->from == 1)
            {
                // The active lane is empty, no need to start timer, just change the lane
                if (car_queue_zero.empty() && active_cars == 0)
                {
                    // restart carHasPassed
                    active_lane = 1;
                    cvDirectionOne.notifyAll();
                    goto beginning;
                    // DO SOMETHING
                }
                // First waiting car starts the timer
                else if (car_queue_one.front() == carId)
                {
                    struct timeval tv;
                    struct timespec ts;
                    gettimeofday(&tv, NULL);
                    ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                    ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                    if (ts.tv_nsec >= 1000000000UL)
                    {
                        ts.tv_nsec -= 1000000000;
                        ts.tv_sec += 1;
                    }

                    int rv = cvTimerOne.timedwait(&ts);
                    if (rv == ETIMEDOUT)
                    {
                        // restart carHasPassed
                        active_lane = 1;
                        cvDirectionOne.notifyAll();
                        goto beginning;
                        // DO SOMETHING AFTER TIMEOUT
                    }

                    goto beginning;
                } // There's nothing to do, wait
                else
                {
                    cvDirectionOne.wait();
                    goto beginning;
                }
            }
        }

        // the active lane is my direction
        if (direction->from == 0)
        {
            // check if the order is you
            while (car_queue_zero.front() != carId)
            {
                // printf("CarId: %d waiting for active_lane || car_queue_front. active_lane: %d\n", carId, active_lane);
                cvOrderZero.wait();
            }

            if (active_lane != 0)
                goto beginning;

            // lane_changed_zero:
            //  if the direction just changed, wait for the last car from other direction if there is any
            //  last_car_passing_zero -> cars from direction one set it and vice versa
            while (last_car_passing_zero)
            {
                // printf("CarId: %d waiting for active_lane || last_car_passing_zero. active_lane: %d\n", carId, active_lane);
                cvCarPassingZero.wait();
            }

            if (active_lane != 0)
                goto beginning;

            // another car from my direction started passing before me, sleep PASS_DELAY
            if (active_cars != 0)
            {
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (PASS_DELAY / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((PASS_DELAY % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                cvPassDelay.timedwait(&ts);
            }

            // need to check again if lane changed during my wait (other lane timed out maybe?)
            if (active_lane != 0)
            {
                goto beginning;
            }

            WriteOutput(carId, 'N', this->id, START_PASSING);

            active_cars++;
            // pop out of queue and notify for cars waiting their orders
            car_queue_zero.pop();

            // if I am the last car on my direction, notify the opposite lane
            // if (car_queue_zero.empty())
            // {
            //     cvTimerOne.notify();
            //     cvDirectionOne.notifyAll();
            // }

            // if the lane changes, flag direction one that a car is passing
            last_car_passing_one = true;

            cvOrderZero.notifyAll();

            // sleep for travel_time
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvTravelDelay.timedwait(&ts);

            active_cars--;
            // if the lane changes, flag direction one that no car left
            if (active_cars == 0)
            {
                last_car_passing_one = false;
                cvTimerOne.notify();
                cvDirectionOne.notifyAll();
                cvOrderOne.notifyAll();
                cvCarPassingOne.notifyAll();
            }

            WriteOutput(carId, 'N', this->id, FINISH_PASSING);
        }
        else if (direction->from == 1)
        {
            // check if the order is you
            while (car_queue_one.front() != carId)
            {
                cvOrderOne.wait();
            }

            if (active_lane != 1)
                goto beginning;

            // lane_changed_one:
            //  if the direction just changed, wait for the last car from other direction if there is any
            //  last_car_passing_zero -> cars from direction one set it and vice versa
            while (last_car_passing_one)
            {
                cvCarPassingOne.wait();
            }

            if (active_lane != 1)
                goto beginning;
            // another car from my direction started passing before me, sleep PASS_DELAY
            if (active_cars != 0)
            {
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (PASS_DELAY / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((PASS_DELAY % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                cvPassDelay.timedwait(&ts);
            }

            // need to check again if lane changed during my wait (other lane timed out maybe?)
            if (active_lane != 1)
            {
                goto beginning;
            }

            WriteOutput(carId, 'N', this->id, START_PASSING);

            active_cars++;
            // pop out of queue and notify for cars waiting their orders
            car_queue_one.pop();

            // if I am the last car on my direction, notify the opposite lane
            // if (car_queue_one.empty())
            // {
            //     cvTimerZero.notify();
            //     cvDirectionZero.notifyAll();
            // }

            // if the lane changes, flag direction zero that a car is passing
            last_car_passing_zero = true;

            cvOrderOne.notifyAll();

            // sleep for travel_time
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvTravelDelay.timedwait(&ts);

            active_cars--;
            // if the lane changes, flag direction one that no car left
            if (active_cars == 0)
            {
                last_car_passing_zero = false;
                cvTimerZero.notify();
                cvDirectionZero.notifyAll();
                cvOrderZero.notifyAll();
                cvCarPassingZero.notifyAll();
            }

            WriteOutput(carId, 'N', this->id, FINISH_PASSING);
        }
    }
};

class Ferry : public Monitor
{
    Condition cvReadyZero, cvReadyOne, cvPassWait, cvTimerZero, cvTimerOne, cvDepartZero, cvDepartOne;
    int num_boarded_zero = 0, num_boarded_one = 0;
    bool max_wait_timeout_zero = false, max_wait_timeout_one = false;
    bool departingZero = false, departingOne = false;
    // pthread_t timer_zero_t, timer_one_t;

public:
    int id, travel_time, max_wait_time, capacity;

    Ferry(int id, int travel_time, int max_wait_time, int capacity) : cvReadyZero(this), cvReadyOne(this), cvPassWait(this), cvTimerZero(this), cvTimerOne(this), cvDepartZero(this), cvDepartOne(this)
    {
        this->id = id;
        this->travel_time = travel_time;
        this->max_wait_time = max_wait_time;
        this->capacity = capacity;
    }

    void pass_ferry(Direction *direction, int carId)
    {
        __synchronized__;
        WriteOutput(carId, 'F', this->id, ARRIVE);

        if (direction->from == 0)
        {
            while (departingZero || num_boarded_zero == capacity)
            {
                cvDepartZero.wait();
            }

            // Once the current ferry departs, load the car
            num_boarded_zero++;
            if (num_boarded_zero == 1)
            {
                // printf("CAR: %d IS STARTING THE TIMER\n", carId);
                //  pthread_create(&timer_zero_t, NULL, timer_zero, this);
                //  pthread_detach(timer_zero_t);

                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                int rv = cvTimerZero.timedwait(&ts);
                if (rv == ETIMEDOUT)
                {
                    // printf("CAR: %d TIMEOUT\n", carId);
                    max_wait_timeout_zero = true;
                    departingZero = true;
                    cvReadyZero.notifyAll();
                }

                goto ferry_out_of_timer_zero;
            }

            if (num_boarded_zero == capacity)
            {
                departingZero = true;
                cvTimerZero.notify();
                cvReadyZero.notifyAll();
            }

            while (!max_wait_timeout_zero && !(departingZero))
            {
                cvReadyZero.wait();
            }

        ferry_out_of_timer_zero:
            num_boarded_zero--;
            WriteOutput(carId, 'F', this->id, START_PASSING);
            // printf("Num boarded is: %d\n", num_boarded_zero);

            if (num_boarded_zero == 0)
            {
                departingZero = false;
                max_wait_timeout_zero = false;
                cvDepartZero.notifyAll();
            }

            // sleep_milli(this->travel_time);
            /* TESTING SLEEP */
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvPassWait.timedwait(&ts);

            WriteOutput(carId, 'F', this->id, FINISH_PASSING);
        }
        else
        {
            while (departingOne || num_boarded_one == capacity)
            {
                cvDepartOne.wait();
            }

            // Once the current ferry departs load the car
            num_boarded_one++;
            if (num_boarded_one == 1)
            {
                // printf("CAR: %d IS STARTING THE TIMER\n", carId);
                //  pthread_create(&timer_zero_t, NULL, timer_zero, this);
                //  pthread_detach(timer_zero_t);

                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                int rv = cvTimerOne.timedwait(&ts);
                if (rv == ETIMEDOUT)
                {
                    // printf("CAR: %d TIMEOUT\n", carId);
                    max_wait_timeout_one = true;
                    departingOne = true;
                    cvReadyOne.notifyAll();
                }

                goto ferry_out_of_timer_one;
            }

            if (num_boarded_one == capacity)
            {
                departingOne = true;
                cvTimerOne.notify();
                cvReadyOne.notifyAll();
            }

            while (!max_wait_timeout_one && !(departingOne))
            {
                cvReadyOne.wait();
            }

        ferry_out_of_timer_one:
            num_boarded_one--;
            WriteOutput(carId, 'F', this->id, START_PASSING);

            if (num_boarded_one == 0)
            {
                departingOne = false;
                max_wait_timeout_one = false;
                cvDepartOne.notifyAll();
            }

            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvPassWait.timedwait(&ts);

            WriteOutput(carId, 'F', this->id, FINISH_PASSING);
        }
    }
};

class Crossroad : public Monitor
{
    Condition cvPassDelay, cvTravelDelay,
        cvDirectionZero, cvDirectionOne, cvDirectionTwo, cvDirectionThree,
        cvTimerZero, cvTimerOne, cvTimerTwo, cvTimerThree,
        cvOrderZero, cvOrderOne, cvOrderTwo, cvOrderThree,
        cvCarPassingZero, cvCarPassingOne, cvCarPassingTwo, cvCarPassingThree;
    bool last_car_passing_zero = false, last_car_passing_one = false, last_car_passing_two = false, last_car_passing_three = false;
    int active_lane = 0, active_cars = 0;
    queue<int> car_queue_zero, car_queue_one, car_queue_two, car_queue_three;

public:
    int id, travel_time, max_wait_time;
    Crossroad(int id, int travel_time, int max_wait_time) : cvPassDelay(this), cvTravelDelay(this),
                                                            cvDirectionZero(this), cvDirectionOne(this), cvDirectionTwo(this), cvDirectionThree(this),
                                                            cvTimerZero(this), cvTimerOne(this), cvTimerTwo(this), cvTimerThree(this),
                                                            cvOrderZero(this), cvOrderOne(this), cvOrderTwo(this), cvOrderThree(this),
                                                            cvCarPassingZero(this), cvCarPassingOne(this), cvCarPassingTwo(this), cvCarPassingThree(this)
    {
        this->id = id;
        this->travel_time = travel_time;
        this->max_wait_time = max_wait_time;
    }

    bool isActiveLaneQueueEmpty()
    {
        switch (active_lane)
        {
        case 0:
            return car_queue_zero.empty();
        case 1:
            return car_queue_one.empty();
        case 2:
            return car_queue_two.empty();
        case 3:
            return car_queue_three.empty();
        default:
            return false;
        }
    }

    void changeActiveLane()
    {
        do
        {
            active_lane = (active_lane + 1) % 4;
        } while (isActiveLaneQueueEmpty());

        // printf("ACTIVE LANE CHANGED, NEW ACTIVE_LANE = %d\n", active_lane);
    }

    void notifyActiveLaneDirection()
    {
        switch (active_lane)
        {
        case 0:
            cvDirectionZero.notifyAll();
            break;
        case 1:
            cvDirectionOne.notifyAll();
            break;
        case 2:
            cvDirectionTwo.notifyAll();
            break;
        case 3:
            cvDirectionThree.notifyAll();
            break;
        }

        cvTimerZero.notify();
        cvTimerOne.notify();
        cvTimerTwo.notify();
        cvTimerThree.notify();
    }

    void notifyOtherLanesNoWaitingLeft()
    {
        last_car_passing_zero = false;
        last_car_passing_one = false;
        last_car_passing_two = false;
        last_car_passing_three = false;

        cvTimerZero.notify();
        cvTimerOne.notify();
        cvTimerTwo.notify();
        cvTimerThree.notify();

        cvDirectionZero.notifyAll();
        cvDirectionOne.notifyAll();
        cvDirectionTwo.notifyAll();
        cvDirectionThree.notifyAll();

        cvOrderZero.notifyAll();
        cvOrderOne.notifyAll();
        cvOrderTwo.notifyAll();
        cvOrderThree.notifyAll();

        cvCarPassingZero.notifyAll();
        cvCarPassingOne.notifyAll();
        cvCarPassingTwo.notifyAll();
        cvCarPassingThree.notifyAll();

        // switch (active_lane)
        // {
        // case 0:
        //     last_car_passing_one = false;
        //     last_car_passing_two = false;
        //     last_car_passing_three = false;

        //     cvTimerOne.notify();
        //     cvTimerTwo.notify();
        //     cvTimerThree.notify();
        //     cvDirectionOne.notifyAll();
        //     cvDirectionTwo.notifyAll();
        //     cvDirectionThree.notifyAll();
        //     cvOrderOne.notifyAll();
        //     cvOrderTwo.notifyAll();
        //     cvOrderThree.notifyAll();
        //     cvCarPassingOne.notifyAll();
        //     cvCarPassingTwo.notifyAll();
        //     cvCarPassingThree.notifyAll();
        //     break;
        // case 1:
        //     last_car_passing_zero = false;
        //     last_car_passing_two = false;
        //     last_car_passing_three = false;

        //     cvTimerZero.notify();
        //     cvTimerTwo.notify();
        //     cvTimerThree.notify();
        //     cvDirectionZero.notifyAll();
        //     cvDirectionTwo.notifyAll();
        //     cvDirectionThree.notifyAll();
        //     cvOrderZero.notifyAll();
        //     cvOrderTwo.notifyAll();
        //     cvOrderThree.notifyAll();
        //     cvCarPassingZero.notifyAll();
        //     cvCarPassingTwo.notifyAll();
        //     cvCarPassingThree.notifyAll();
        //     break;
        // case 2:
        //     last_car_passing_one = false;
        //     last_car_passing_zero = false;
        //     last_car_passing_three = false;

        //     cvTimerOne.notify();
        //     cvTimerZero.notify();
        //     cvTimerThree.notify();
        //     cvDirectionOne.notifyAll();
        //     cvDirectionZero.notifyAll();
        //     cvDirectionThree.notifyAll();
        //     cvOrderOne.notifyAll();
        //     cvOrderZero.notifyAll();
        //     cvOrderThree.notifyAll();
        //     cvCarPassingOne.notifyAll();
        //     cvCarPassingZero.notifyAll();
        //     cvCarPassingThree.notifyAll();
        //     break;
        // case 3:
        //     last_car_passing_one = false;
        //     last_car_passing_two = false;
        //     last_car_passing_zero = false;

        //     cvTimerOne.notify();
        //     cvTimerTwo.notify();
        //     cvTimerZero.notify();
        //     cvDirectionOne.notifyAll();
        //     cvDirectionTwo.notifyAll();
        //     cvDirectionZero.notifyAll();
        //     cvOrderOne.notifyAll();
        //     cvOrderTwo.notifyAll();
        //     cvOrderZero.notifyAll();
        //     cvCarPassingOne.notifyAll();
        //     cvCarPassingTwo.notifyAll();
        //     cvCarPassingZero.notifyAll();
        //     break;
        // }
    }

    void pass_crossroad(Direction *direction, int carId)
    {
        __synchronized__;
        WriteOutput(carId, 'C', this->id, ARRIVE);

        // drive into lane queue
        switch (direction->from)
        {
        case 0:
            car_queue_zero.push(carId);
            break;
        case 1:
            car_queue_one.push(carId);
            break;
        case 2:
            car_queue_two.push(carId);
            break;
        case 3:
            car_queue_three.push(carId);
            break;
        }

    beginning_crossroad:
        // if currently active lane is not my direction
        if (active_lane != direction->from)
        {
            // The active lane is empty, no need to start timer, just change the lane
            if (isActiveLaneQueueEmpty() && active_cars == 0)
            {
                // printf("CarID: %d, active lane = %d, empty changing direction\n", carId, active_lane);
                changeActiveLane();
                notifyActiveLaneDirection();
                goto beginning_crossroad;
            }

            if (direction->from == 0)
            {
                // First waiting car starts the timer
                if (car_queue_zero.front() == carId)
                {
                    struct timeval tv;
                    struct timespec ts;
                    gettimeofday(&tv, NULL);
                    ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                    ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                    if (ts.tv_nsec >= 1000000000UL)
                    {
                        ts.tv_nsec -= 1000000000;
                        ts.tv_sec += 1;
                    }

                    // printf("CarID: %d, starting timer for direction %d\n", carId, direction->from);

                    int rv = cvTimerZero.timedwait(&ts);
                    if (rv == ETIMEDOUT)
                    {
                        // printf("CarID: %d, TIMEOUT for direction %d\n", carId, direction->from);

                        // restart carHasPassed
                        changeActiveLane();
                        notifyActiveLaneDirection();
                        goto beginning_crossroad;
                        // DO SOMETHING AFTER TIMEOUT
                    }

                    // printf("CarID: %d, NO TIMEOUT for direction %d\n", carId, direction->from);

                    goto beginning_crossroad;
                }
                // Nothing to do, wait
                else
                {
                    // printf("CarID: %d, waiting for direction %d\n", carId, direction->from);
                    cvDirectionZero.wait();
                    goto beginning_crossroad;
                }
            }
            else if (direction->from == 1)
            {
                // First waiting car starts the timer
                if (car_queue_one.front() == carId)
                {
                    struct timeval tv;
                    struct timespec ts;
                    gettimeofday(&tv, NULL);
                    ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                    ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                    if (ts.tv_nsec >= 1000000000UL)
                    {
                        ts.tv_nsec -= 1000000000;
                        ts.tv_sec += 1;
                    }

                    // printf("CarID: %d, starting timer for direction %d\n", carId, direction->from);

                    int rv = cvTimerOne.timedwait(&ts);
                    if (rv == ETIMEDOUT)
                    {
                        // restart carHasPassed
                        // printf("CarID: %d, TIMEOUT for direction %d\n", carId, direction->from);

                        changeActiveLane();
                        notifyActiveLaneDirection();
                        goto beginning_crossroad;
                        // DO SOMETHING AFTER TIMEOUT
                    }

                    // printf("CarID: %d, NO TIMEOUT for direction %d\n", carId, direction->from);

                    goto beginning_crossroad;
                }
                // Nothing to do, wait
                else
                {
                    // printf("CarID: %d, waiting for direction %d\n", carId, direction->from);

                    cvDirectionOne.wait();
                    goto beginning_crossroad;
                }
            }
            else if (direction->from == 2)
            {
                // First waiting car starts the timer
                if (car_queue_two.front() == carId)
                {
                    struct timeval tv;
                    struct timespec ts;
                    gettimeofday(&tv, NULL);
                    ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                    ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                    if (ts.tv_nsec >= 1000000000UL)
                    {
                        ts.tv_nsec -= 1000000000;
                        ts.tv_sec += 1;
                    }

                    // printf("CarID: %d, starting timer for direction %d\n", carId, direction->from);
                    int rv = cvTimerTwo.timedwait(&ts);

                    if (rv == ETIMEDOUT)
                    {
                        // restart carHasPassed
                        // printf("CarID: %d, TIMEOUT for direction %d\n", carId, direction->from);

                        changeActiveLane();
                        notifyActiveLaneDirection();
                        goto beginning_crossroad;
                        // DO SOMETHING AFTER TIMEOUT
                    }

                    // printf("CarID: %d, NO TIMEOUT for direction %d\n", carId, direction->from);

                    goto beginning_crossroad;
                }
                // Nothing to do, wait
                else
                {
                    // printf("CarID: %d, waiting for direction %d\n", carId, direction->from);

                    cvDirectionTwo.wait();
                    goto beginning_crossroad;
                }
            }
            else if (direction->from == 3)
            {
                // First waiting car starts the timer
                if (car_queue_three.front() == carId)
                {
                    struct timeval tv;
                    struct timespec ts;
                    gettimeofday(&tv, NULL);
                    ts.tv_sec = tv.tv_sec + (this->max_wait_time / 1000);
                    ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->max_wait_time % 1000))) * 1000UL;
                    if (ts.tv_nsec >= 1000000000UL)
                    {
                        ts.tv_nsec -= 1000000000;
                        ts.tv_sec += 1;
                    }

                    // printf("CarID: %d, starting timer for direction %d\n", carId, direction->from);
                    int rv = cvTimerThree.timedwait(&ts);

                    if (rv == ETIMEDOUT)
                    {
                        // restart carHasPassed
                        // printf("CarID: %d, TIMEOUT for direction %d\n", carId, direction->from);

                        changeActiveLane();
                        notifyActiveLaneDirection();
                        goto beginning_crossroad;
                        // DO SOMETHING AFTER TIMEOUT
                    }

                    // printf("CarID: %d, NO TIMEOUT for direction %d\n", carId, direction->from);

                    goto beginning_crossroad;
                }
                // Nothing to do, wait
                else
                {
                    // printf("CarID: %d, waiting for direction %d\n", carId, direction->from);

                    cvDirectionThree.wait();
                    goto beginning_crossroad;
                }
            }
        }

        // the active lane is my direction
        if (direction->from == 0)
        {
            // check if the order is you
            while (car_queue_zero.front() != carId)
            {
                // printf("CarID: %d, waiting for order at lane 0\n", carId);
                cvOrderZero.wait();
            }

            // printf("CarID: %d is at the front for lane 0\n", carId);
            if (active_lane != 0)
                goto beginning_crossroad;

            // lane_changed_zero_crossroad:
            //  if the direction just changed, wait for the last car from other direction if there is any
            //  last_car_passing_zero -> cars from direction one set it and vice versa
            while (last_car_passing_zero)
            {
                // printf("CarID: %d, waiting for last_car_passing at 0\n", carId);
                cvCarPassingZero.wait();
            }

            // printf("CarID: %d no last car passing for lane 0\n", carId);
            if (active_lane != 0)
                goto beginning_crossroad;

            // another car from my direction started passing before me, sleep PASS_DELAY
            if (active_cars != 0)
            {
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (PASS_DELAY / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((PASS_DELAY % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                cvPassDelay.timedwait(&ts);
            }

            // need to check again if lane changed during my wait (other lane timed out maybe?)
            if (active_lane != 0)
            {
                goto beginning_crossroad;
            }

            WriteOutput(carId, 'C', this->id, START_PASSING);

            active_cars++;
            // pop out of queue and notify for cars waiting their orders
            car_queue_zero.pop();

            // if I am the last car on my direction, notify the opposite lane
            // if (car_queue_zero.empty())
            // {
            //     cvTimerOne.notify();
            //     cvDirectionOne.notifyAll();
            // }

            // if the lane changes, flag other directions that a car is passing
            last_car_passing_one = true;
            last_car_passing_two = true;
            last_car_passing_three = true;

            cvOrderZero.notifyAll();

            // sleep for travel_time
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvTravelDelay.timedwait(&ts);

            active_cars--;
            // if the lane changes, flag direction one that no car left
            if (active_cars == 0)
            {
                notifyOtherLanesNoWaitingLeft();
            }

            WriteOutput(carId, 'C', this->id, FINISH_PASSING);
        }
        else if (direction->from == 1)
        {
            // check if the order is you
            while (car_queue_one.front() != carId)
            {
                // printf("CarID: %d, waiting for order at lane 1\n", carId);

                cvOrderOne.wait();
            }
            // printf("CarID: %d is at the front for lane 1\n", carId);

            if (active_lane != 1)
                goto beginning_crossroad;

            // if the direction just changed, wait for the last car from other direction if there is any
            // last_car_passing_zero -> cars from direction one set it and vice versa
            while (last_car_passing_one)
            {
                // printf("CarID: %d, waiting for last_car_passing at 1\n", carId);

                cvCarPassingOne.wait();
            }

            // printf("CarID: %d no last car passing for lane 1\n", carId);
            if (active_lane != 1)
                goto beginning_crossroad;

            // another car from my direction started passing before me, sleep PASS_DELAY
            if (active_cars != 0)
            {
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (PASS_DELAY / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((PASS_DELAY % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                cvPassDelay.timedwait(&ts);
            }

            // need to check again if lane changed during my wait (other lane timed out maybe?)
            if (active_lane != 1)
            {
                goto beginning_crossroad;
            }

            WriteOutput(carId, 'C', this->id, START_PASSING);

            active_cars++;
            // pop out of queue and notify for cars waiting their orders
            car_queue_one.pop();

            // if I am the last car on my direction, notify the opposite lane
            // if (car_queue_zero.empty())
            // {
            //     cvTimerOne.notify();
            //     cvDirectionOne.notifyAll();
            // }

            // if the lane changes, flag other directions that a car is passing
            last_car_passing_zero = true;
            last_car_passing_two = true;
            last_car_passing_three = true;

            cvOrderOne.notifyAll();

            // sleep for travel_time
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvTravelDelay.timedwait(&ts);

            active_cars--;
            // if the lane changes, flag direction one that no car left
            if (active_cars == 0)
            {
                notifyOtherLanesNoWaitingLeft();
            }

            WriteOutput(carId, 'C', this->id, FINISH_PASSING);
        }
        else if (direction->from == 2)
        {
            // check if the order is you
            while (car_queue_two.front() != carId)
            {
                // printf("CarID: %d, waiting for order at lane 2\n", carId);

                cvOrderTwo.wait();
            }
            // printf("CarID: %d is at the front for lane 2\n", carId);

            if (active_lane != 2)
                goto beginning_crossroad;

            // if the direction just changed, wait for the last car from other direction if there is any
            // last_car_passing_zero -> cars from direction one set it and vice versa
            while (last_car_passing_two)
            {
                // printf("CarID: %d, waiting for last_car_passing at 2\n", carId);
                cvCarPassingTwo.wait();
            }

            // printf("CarID: %d no last car passing for lane 2\n", carId);
            if (active_lane != 2)
                goto beginning_crossroad;

            // another car from my direction started passing before me, sleep PASS_DELAY
            if (active_cars != 0)
            {
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (PASS_DELAY / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((PASS_DELAY % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                cvPassDelay.timedwait(&ts);
            }

            // need to check again if lane changed during my wait (other lane timed out maybe?)
            if (active_lane != 2)
            {
                goto beginning_crossroad;
            }

            WriteOutput(carId, 'C', this->id, START_PASSING);

            active_cars++;
            // pop out of queue and notify for cars waiting their orders
            car_queue_two.pop();

            // if I am the last car on my direction, notify the opposite lane
            // if (car_queue_zero.empty())
            // {
            //     cvTimerOne.notify();
            //     cvDirectionOne.notifyAll();
            // }

            // if the lane changes, flag other directions that a car is passing
            last_car_passing_one = true;
            last_car_passing_zero = true;
            last_car_passing_three = true;

            cvOrderTwo.notifyAll();

            // sleep for travel_time
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvTravelDelay.timedwait(&ts);

            active_cars--;
            // if the lane changes, flag direction one that no car left
            if (active_cars == 0)
            {
                notifyOtherLanesNoWaitingLeft();
            }

            WriteOutput(carId, 'C', this->id, FINISH_PASSING);
        }
        else if (direction->from == 3)
        {
            // check if the order is you
            while (car_queue_three.front() != carId)
            {
                // printf("CarID: %d, waiting for order at lane 3\n", carId);

                cvOrderThree.wait();
            }

            // printf("CarID: %d is at the front for lane 3\n", carId);
            if (active_lane != 3)
                goto beginning_crossroad;

            // if the direction just changed, wait for the last car from other direction if there is any
            // last_car_passing_zero -> cars from direction one set it and vice versa
            while (last_car_passing_three)
            {
                // printf("CarID: %d, waiting for last_car_passing at 3\n", carId);

                cvCarPassingThree.wait();
            }

            // printf("CarID: %d no last car passing for lane 3\n", carId);
            if (active_lane != 3)
                goto beginning_crossroad;

            // another car from my direction started passing before me, sleep PASS_DELAY
            if (active_cars != 0)
            {
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + (PASS_DELAY / 1000);
                ts.tv_nsec = (tv.tv_usec + 1000UL * ((PASS_DELAY % 1000))) * 1000UL;
                if (ts.tv_nsec >= 1000000000UL)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec += 1;
                }

                cvPassDelay.timedwait(&ts);
            }

            // need to check again if lane changed during my wait (other lane timed out maybe?)
            if (active_lane != 3)
            {
                goto beginning_crossroad;
            }

            WriteOutput(carId, 'C', this->id, START_PASSING);

            active_cars++;
            // pop out of queue and notify for cars waiting their orders
            car_queue_three.pop();

            // if I am the last car on my direction, notify the opposite lane
            // if (car_queue_zero.empty())
            // {
            //     cvTimerOne.notify();
            //     cvDirectionOne.notifyAll();
            // }

            // if the lane changes, flag other directions that a car is passing
            last_car_passing_one = true;
            last_car_passing_two = true;
            last_car_passing_zero = true;

            cvOrderThree.notifyAll();

            // sleep for travel_time
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + (this->travel_time / 1000);
            ts.tv_nsec = (tv.tv_usec + 1000UL * ((this->travel_time % 1000))) * 1000UL;
            if (ts.tv_nsec >= 1000000000UL)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec += 1;
            }
            cvTravelDelay.timedwait(&ts);

            active_cars--;
            // if the lane changes, flag direction one that no car left
            if (active_cars == 0)
            {
                notifyOtherLanesNoWaitingLeft();
            }

            WriteOutput(carId, 'C', this->id, FINISH_PASSING);
        }
    }
};

// Path struct
class Path
{
public:
    Path(Monitor *conn, int ctype, Direction *dir)
    {
        this->connector = conn;
        this->connector_type = ctype;
        this->direction = dir;
    }

    Monitor *connector;
    int connector_type;
    Direction *direction;
    NarrowBridge *get_narrow_bridge() { return (NarrowBridge *)this->connector; }

    Ferry *get_ferry() { return (Ferry *)this->connector; }

    Crossroad *get_crossroad() { return (Crossroad *)this->connector; }
};

/* START OF PASS FUNCTIONS */

void Pass(Ferry *connector, Direction *direction, int id)
{
    connector->pass_ferry(direction, id);
}

void Pass(NarrowBridge *connector, Direction *direction, int id)
{
    connector->pass_narrow_bridge(direction, id);
}

void Pass(Crossroad *connector, Direction *direction, int id)
{
    connector->pass_crossroad(direction, id);
}

/* END OF PASS FUNCTIONS*/

class Car
{
public:
    int id, travel_time, path_length;
    vector<Path> *path_arr;

    Car(int id, int travel_time, int path_length, vector<Path> *path_arr)
    {
        this->id = id;
        this->travel_time = travel_time;
        this->path_length = path_length;
        this->path_arr = path_arr;
    }

    void start()
    {
        pthread_create(&car_tid, nullptr, car_routine, this);
    }

    void join() const
    {
        pthread_join(car_tid, nullptr);
    }

private:
    pthread_t car_tid;

    static void *car_routine(void *arg)
    {
        Car *car = reinterpret_cast<Car *>(arg);
        int path_length = car->path_length, id = car->id, travel_time = car->travel_time;
        vector<Path> *path_arr = car->path_arr;

        for (int path_idx = 0; path_idx < path_length; path_idx++)
        {
            Path curr_path = path_arr->at(path_idx);

            switch (curr_path.connector_type)
            {
            case NARROW_BRIDGE:
            {
                NarrowBridge *nb = curr_path.get_narrow_bridge();

                WriteOutput(id, 'N', nb->id, TRAVEL);
                sleep_milli(travel_time);
                Pass(nb, curr_path.direction, car->id);
                break;
            }
            case FERRY:
            {
                Ferry *f = curr_path.get_ferry();

                WriteOutput(id, 'F', f->id, TRAVEL);
                sleep_milli(travel_time);
                Pass(f, curr_path.direction, car->id);
                break;
            }
            case CROSSROAD:
            {
                Crossroad *c = curr_path.get_crossroad();

                WriteOutput(id, 'C', c->id, TRAVEL);
                sleep_milli(travel_time);
                Pass(c, curr_path.direction, car->id);
                break;
            }
            }
        }

        pthread_exit(nullptr);
    }
};

/* END OF CLASSES */

int main()
{
    auto *nb_arr = new vector<NarrowBridge *>();
    auto *ferry_arr = new vector<Ferry *>();
    auto *crossroad_arr = new vector<Crossroad *>();
    auto *car_arr = new vector<Car *>();

    // Parse for NarrowBridge
    int numOfNarrowBridge;
    cin >> numOfNarrowBridge;
    for (int i = 0; i < numOfNarrowBridge; i++)
    {
        int travel_time, max_wait_time;
        cin >> travel_time >> max_wait_time;

        nb_arr->push_back(new NarrowBridge(i, travel_time, max_wait_time));
    }

    // Parse for Ferry
    int numOfFerry;
    cin >> numOfFerry;
    for (int i = 0; i < numOfFerry; i++)
    {
        int travel_time, max_wait_time, capacity;
        cin >> travel_time >> max_wait_time >> capacity;

        ferry_arr->push_back(new Ferry(i, travel_time, max_wait_time, capacity));
    }

    // Parse for Crossroad
    int numOfCrossroad;
    cin >> numOfCrossroad;
    for (int i = 0; i < numOfCrossroad; i++)
    {
        int travel_time, max_wait_time;
        cin >> travel_time >> max_wait_time;

        crossroad_arr->push_back(new Crossroad(i, travel_time, max_wait_time));
    }

    // Parse for Car
    int numOfCar;
    cin >> numOfCar;
    for (int i = 0; i < numOfCar; i++)
    {
        int travel_time, path_length;
        cin >> travel_time >> path_length;
        auto path_arr = new vector<Path>();

        for (int j = 0; j < path_length; j++)
        {
            string connector_str;
            cin >> connector_str;
            int connector_id = connector_str.at(1) - '0';

            auto *path_direction = new Direction;
            cin >> path_direction->from >> path_direction->to;

            switch (connector_str.at(0))
            {
            case 'N':
                path_arr->emplace_back(nb_arr->at(connector_id), 0, path_direction);
                break;
            case 'F':
                path_arr->emplace_back(ferry_arr->at(connector_id), 1, path_direction);
                break;
            case 'C':
                path_arr->emplace_back(crossroad_arr->at(connector_id), 2, path_direction);
                break;
            }
        }
        car_arr->push_back(new Car(i, travel_time, path_length, path_arr));
    }

    // Start the car threads
    InitWriteOutput();
    for (int i = 0; i < numOfCar; i++)
    {
        car_arr->at(i)->start();
    }

    // Wait for the car threads to finish
    for (int i = 0; i < numOfCar; i++)
    {
        car_arr->at(i)->join();
    }

    exit(0);
}
