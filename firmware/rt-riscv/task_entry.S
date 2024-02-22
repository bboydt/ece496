.global rt_task_entry
rt_task_entry:

    jalr a0
0:  j 0b
    j rt_task_exit

.global rt_task_entry_arg
rt_task_entry_arg:

    # we expect the task's arg to be in a0
    jalr a1
    j rt_task_exit

