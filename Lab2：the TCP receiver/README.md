刚初始化项目就报错了：

```sh
$ make
--   NOTE: You can choose a build type by calling cmake with one of:
--     -DCMAKE_BUILD_TYPE=Release   -- full optimizations
--     -DCMAKE_BUILD_TYPE=Debug     -- better debugging experience in gdb
--     -DCMAKE_BUILD_TYPE=RelASan   -- full optimizations plus address and undefined-behavior sanitizers
--     -DCMAKE_BUILD_TYPE=DebugASan -- debug plus sanitizers
CMake Error: The following variables are used in this project, but they are set to NOTFOUND.
Please set them or make sure they are set and tested correctly in the CMake files:
LIBPCAP
    linked by target "tcp_parser" in directory /root/sponge/tests
    linked by target "tcp_parser" in directory /root/sponge/tests

-- Configuring incomplete, errors occurred!
See also "/root/sponge/build/CMakeFiles/CMakeOutput.log".
make: *** [cmake_check_build_system] Error 1
```

咱也不太懂Cmake的东西，猜想是之前用VS Code远程调试，可能不小心搞乱了什么文件，只好用万能的“remake”了。

把文件夹删了，重新`git clone`了Lab文件夹，然后把之前写好的代码直接复制过去，再编译Lab2，还是一样。

网上找了一圈也没找到个解决方法，于是去查了查之前收集的别人的CS144学习笔记，在[这一篇](https://www.cnblogs.com/kangyupl/p/stanford_cs144_labs.html)中找到了解决方法，安装好`libpcap-devel`库，终于成功编译。

出师不利啊……

### Translating between 64-bit indexes and 32-bit seqnos
#### 实验大意
实现64位的**流索引(Stream Index)**、32位的**序列号(Sequence Numbers/Seqno)**、32位的**绝对序列号(Absolute Sequence Numbers/Absolute Seqno)** 之间的转换。

根据实验讲义，假设传输的数据为字符串`cat`，seqno从`2^32-2`开始，则三者的关系如下：

|element        |SYN   |c     |a|t|FIN|
|---------------|------|------|-|-|---|
|seqno          |2^32-2|2^32-1|0|1|2  |
|absolute seqno |0     |1     |2|3|4  |
|stream index   |  ——  |0     |1|2|—— |

本实验主要完成`seqno`与`absolute seqno`之间的相互转换。

首先要明确范围，`seqno`是32位无符号整数，超出2^32位后溢出，再次从0开始计数。

`absolute seqno`是64位无符号整数，范围相当大，讲义中说可以假设其不会溢出。

可以得出转换关系:
$$
seqno = (absolute\ seqno) mod (1 >> 32) + isn
$$

#### wrap
实现`absolute seqno`到`seqno`的转换。

由于`absolute seqno`的范围比较大，而且无符号32位整数溢出相当于自动模2^32，可以根据公式直接计算。

#### unwrap
实现`seqno`到`absolute seqno`的转换。

由较小范围的数转换为较大范围的数，转换结果不是唯一的，因此给出`checkpoint`，求出与`checkpoint`差值最小的`absolute seqno`的值。

首先求出`isn`到`seqno`的偏移量，记为`offset`，则

$$
offset = (seqno + 2^{32} -isn) mod (2^{32})
$$

由于是无符号整数类型，溢出后实际上相当于自动取余了，可以直接用`seqno-isn`来计算`offset`（当然，offset是`uint32_t`类型的）。

接下来，找出所有可能的`absolute seqno`，记为`x`。计算所有`x`与`offset`的差的绝对值，其中绝对值最小的`x`即为答案。

`absolute seqno`为了加快查找速度，不需要真的找出所有的`absolute seqno`，只要能找到`absolute seqno`前后最近的`x`即可，也就是恰好将`absolute seqno`夹在中间的两个`x`。

先将`checkpoint`整除2^32，再乘2^32，最后加上`offset`，此时得到的一定是两个`x`之一，再判断是比`checkpoint`大的`x`还是比它小的`x`，直接做差比较即可得到答案。

#### 实验结果
```
Test project /root/sponge/build
    Start 1: t_wrapping_ints_cmp
1/4 Test #1: t_wrapping_ints_cmp ..............   Passed    0.00 sec
    Start 2: t_wrapping_ints_unwrap
2/4 Test #2: t_wrapping_ints_unwrap ...........   Passed    0.00 sec
    Start 3: t_wrapping_ints_wrap
3/4 Test #3: t_wrapping_ints_wrap .............   Passed    0.00 sec
    Start 4: t_wrapping_ints_roundtrip
4/4 Test #4: t_wrapping_ints_roundtrip ........   Passed    0.18 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.21 sec
```

#### 小坑
计算2^32次时，使用左移运算符，直接用`1<<32`会溢出，看了测试代码才知道要加上`ul`，即`1ul<<32`，`ul`表示`unsigned long`，即无符号长整数。