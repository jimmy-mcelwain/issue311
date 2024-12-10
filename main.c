#include <unistd.h>
#include <time.h>
#include <rcl/rcl.h>
#include <rcl/types.h>
#include <rcl/publisher.h>

#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rosidl_runtime_c/string_functions.h>
#include <rosidl_runtime_c/primitives_sequence_functions.h>

#include <rmw/qos_profiles.h>
#include <rmw_microros/ping.h>

#include <sensor_msgs/msg/joint_state.h>

#define JOINT_COUNT 15

rcl_timer_t Ros_StateServer_Timer;
rclc_executor_t Ros_StateServer_Executor;

rcl_publisher_t publisherJointState;
sensor_msgs__msg__JointState *msgJointState;

static rcl_allocator_t rclAllocator;

typedef int BOOL;

#define MAX_JOINT_NAME_LENGTH 32

void Ros_StateServer_TimerCallback(rcl_timer_t *timer, int64_t last_call_time)
{
    (void)last_call_time; // unused

    if (timer != NULL)
    {

        rclc_support_t rclSupport;
        rcl_init_options_t opts = rcl_get_zero_initialized_init_options();
        rcl_init_options_init(&opts, rclAllocator);
        rcl_init_options_set_domain_id(&opts, 4);
        rcl_init_options_get_rmw_init_options(&opts);
        rclc_support_init_with_options(&rclSupport, 0, NULL, &opts, &rclAllocator);
        rclc_support_fini(&rclSupport);
        rcl_init_options_fini(&opts);
    }
}

int main()
{
    rclc_support_t rclSupport;
    rcl_node_t rclNode;
    rcl_ret_t ret;

    // Wait for agent to become available
    do
    {
        usleep(1000);
        ret = rmw_uros_ping_agent(1000, 2);
    } while (ret != RCL_RET_OK);

    //=============================================
    rclAllocator = rcl_get_default_allocator();

    rclc_support_init(&rclSupport, 0, NULL, &rclAllocator);
    rclc_node_init_default(&rclNode, "testnode", "", &rclSupport);

    //=============================================
    rclc_timer_init_default(
        &Ros_StateServer_Timer,
        &rclSupport,
        RCL_MS_TO_NS(500),
        Ros_StateServer_TimerCallback);

    Ros_StateServer_Executor = rclc_executor_get_zero_initialized_executor();
    rclc_executor_init(&Ros_StateServer_Executor, &rclSupport.context, 1, &rclAllocator);
    rclc_executor_add_timer(&Ros_StateServer_Executor, &Ros_StateServer_Timer);

    rclc_executor_spin(&Ros_StateServer_Executor);
}
