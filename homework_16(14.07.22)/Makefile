all: task_1 task_2_host task_2_user task_3 rm

task_1: task_1.c
	gcc task_1.c -o task_1

task_2_host: task_2_host.c
	gcc task_2_host.c -o task_2_host

task_2_user: task_2_user.c
	gcc task_2_user.c -o task_2_user

task_3: task_3.c
	gcc task_3.c -o task_3

rm: pipe
	rm -rf pipe

clean:
	rm -rf task_1 task_2_host task_2_user task_3
