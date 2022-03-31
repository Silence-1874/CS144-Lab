==实在做不下去了。最近也是被疫情搞得心情很差，这学期课又多，先放一阵子。准备留着暑假做，但愿flag不倒。==
#### 实验大意
实现一个*流重组器(StreamReassembler)*，处理收到报文段的乱序、重复、重叠等问题。（丢包暂时不用考虑，因为本次实验重点在于接收方）

#### 前期思考:
- 因为对C++的库不熟，一开始想直接用数组来充当哈希表，但index到后面会越来越大，数组空间肯定不够用，只好先去学一下C++的`map`了。
- 确定好使用`map`了，但`map`的键值对应该要存储什么呢？最开始我是打算学类似于操作系统磁盘空闲空间管理，键值对分别存储**空闲区域的起始下标**和**空闲区域的结束下标**，但是在实现的过程中遇到了很多困难。最主要的是每次`write()`有点不知所措，`write()`需要传入字符串，因此必须把每次接收到的字符串存储下来。参考了一些网上的文章，才发现自己想复杂了，`map`直接存储`每个字符的绝对索引`和`这个字符`即可。
- 接下来重点研究讲义里的这张图片。
  ![](https://s2.loli.net/2022/03/30/jl2a84Ag6ZzcbNL.png)
  最初笔者定义的成员变量里包括了`first unread`和`first unassembled`，但是头文件里并没有给出类似于`read`的函数，这样我就无从得知`first unread`该如何变化了。
  重新看了一遍讲义，发现其中有一句

  > The owner can access and read from the ByteStream whenever it wants.

  也就是说，**上层**会**随时**read已经装配好的（绿色）部分。
- 由上图可得，
  **first unread + capacity = first unacceptable**
  如果不确定`first unread`，就无从得知`first unacceptable`了，也就没法判断接收到的报文段是否超出范围，因此还得另辟蹊径。
- 突然发现还有一个熟悉的身影——`ByteStream`。
  我们在`Lab0`中已经实现了它。其中有一个成员变量，`_total_read`（这个变量名是笔者在`Lab0`中自己取的，对应的get函数为`bytes_read()`），表示字节流已经读取的字符数。再根据`index`的映射关系，就可以确定`first unread`了。
  因为何时read是由上层决定的，所以`first unread`应该由`ByteStream`类型的`_output`计算得来。
  同理，`first unassembled`也可以通过`_total_written`（对应get函数`bytes_written()`)计算出来。
  现在已经可以确定图中的三个分界点了。

#### 总体思路
- 用`map`来管理为装配的（红色）部分，键为绝对索引，值为一个个字符。
- 对于新加入的字符串，计算出其绝对索引范围`[lfet, right)`，并去掉尾部超出`capacity`(即绝对下标大于`unacceptable`)的部分。将字符串的字符逐个添加到`map`中，因为`map`会覆盖相同key的value，所以自然就解决了重复和重叠的问题。
- 当`first unassembled`被填充时，从它的绝对索引开始，将后面所有连续的字符一并加入，一起写入`_out_put`。


#### 踩坑历程
- 最开始的几次提交一直报段错误，调试了一下，发现能过第一个代码块的测试，但是到第二个代码块一初始化就错了。

#### 实验总结
感觉自己总是想太多了，可能是算法题刷多了，连基本功能都还没实现，就老想着在时间和空间上优化，浪费了很多时间。
虽然在CSAPP的BombLab中用过GDB，但到现在早就忘光光了，只记得命令行调试很痛苦，于是花了好几个小时尝试用CLion来调试，可惜还是没成功，一直报`com.jetbrains.cidr.execution.debugger.backend.gdb.GDBDriver$GDBCommandException: tcp:192.168.83.129:1234: The system tried to join a drive to a directory on a joined drive.`，搞得我心态都崩了。
尝试曲线救国，换VSCode试试，结果又是好几个小时，也没有配置成功……最后还是老老实实用GDB了。
千方百计想抄近路，结果反而绕了一大圈，还是太浮躁了……