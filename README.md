---
finished_date: 2020-11-08
tags:
    - OS
    - deadlock_detection
    - graph_reduction
    - multiple_resource_type_multiple_resource_unit
    - C
    - linux
---
# Deadlock Detection with Graph Reduction method
- Implement deadlock detection method in C
- find out if the given input is deadlock state or not
- if in deadlock state, find out process in deadlock state
- input file
    - the file includes **pid, arrival time, the number of cycle, cpu burst, i/o burst, initial Q of each process**
    - **the number of process** is also included
- output file
    - gantt chart.txt
    - **waiting time, turnaround time of each process** are included
    - **average waiting time, turnaround time of whoel process** are also included
## Solving strategy
- when required resource > remain resource unit → deadlock
- mark unblocked processes in *finish* array
- if there is a process that has proper condition to be run, mark *change* flage high
- if all processes are checked and *change* is low, finish detection
- if detection is finished, processes whose *finished* is not 1 is in blocked state

- user can use random input mode to observe more variable inputs

## Result
- output of input3.txt
![image](https://user-images.githubusercontent.com/74483608/160060242-74084f17-4034-40c8-900f-dec2b58e93d8.png)
- output of random input 
![image](https://user-images.githubusercontent.com/74483608/160060651-52c40a15-2d6b-43bf-b7ee-8669f900e62c.png)

- input file description
```
number_of_Processes number_of_resource_types number_of_units_for_each_resources
allocation_matrix
required_matrix

ex
2, 2, 3, 2
2, 0
1, 1
0, 1
1, 1

```

## File structure
```
|-- data
    |-- input.txt
    |-- input1.txt
    |-- input2.txt
    |-- input3.txt
|-- src
    |-- deadlock_detection_source.c
|-- deadlock_detection_docuemnt.pdf
|-- test_log.txt
```
## 배운 점
- 한 프로세스가 교착 상태가 된다면 다른 프로세스 또한 교착 상태가 될 확률이 매우 높다는 것을 확인할 수 있었다. (random input에 대한 관찰을 20번 진행하였으며 log는 *test_log.txt*에서 확인할 수 있다.)
- 프로세스가 1개라면 모든 리소스를 혼자 사용하기 때문에 deadlock state가 될 수 없다.
- random input을 관찰하는 기능을 추가하여 다양한 상황에 대한 결과를 관찰할 수 있었다.
## 한계점
- header 파일로 구현한 함수 혹은 자료구조를 처리하여 file을 나눠 관리하였다면 재사용성, 가독성과 관리의 측면에서 도움이 되었을 것이다.
