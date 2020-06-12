# lua-list
在不改动lua的其他语法功能的情况下，在lua中添加链表类型的支持，基于lua5.4.0-rc3。

另外会添加一些其他的基础库函数，以方便后续的使用。新添加的函数参照python中对应函数的使用方式（函数名称，参数顺序等）。

## 基本语法
- 使用[]定义链表

```lua
> a = []
> a
list: 000001F666CE2B70
```

- 使用[]获取list的下标，下标从1开始，负数表示从list的末尾开始

```lua
> a = [3, 4, 5]
> a[1]
3
> a[-1]
5
> a[-1] = 6
> a[-1]
6
```

- 使用#获取list长度

```lua
> a = [1, 2, 3]
> #a
3
```

- 设置超出范围的下标会自动在list开头或结尾添加数据元素，设置下标0表示在开头插入，设置#a + 1表示在结尾插入

```lua
> a[0] = 5
> a[1], a[2], a[3], a[4]
5       1       2       3
> a[#a + 1] = 6
> a[1], a[2], a[3], a[4], a[5]
5       1       2       3       6
```

- 使用+来连接两个list

```lua
> a = [1, 2, 3] + [4, 5]
> a[1], a[2], a[3], a[4], a[5]
1       2       3       4       5
```

- list可以嵌套定义，注意[[表示字符串开始，因此中间需要添加一个空格

```lua
> a = [ []]
> a
list: 000001F666CEC030
> a[1]
list: 000001F666CECA80
```

- list可以和table相互嵌套

```lua
> a = {1, [2, {3, [4]=5}], [6]=7}
> a[1], a[2], a[6]
1       list: 000001B034042C60  7
> a[2][1], a[2][2]
2       table: 000001B034046CC0
> a[2][2][1], a[2][2][4]
3       5
```

- 按索引顺序遍历的时间复杂度为O(n)，随机遍历的时间复杂度仍为O(n^2)

```lua
> a = [1, 2, 3, 4, 5]
> for i = 1, 5 do
>>	print(a[i])
>>end
1
2
3
4
5
```

## list库函数

- ##### list.concat(list [, sep='' [, start=1 [, end=-1]]])

连接list中从start位置到end位置的所有元素, 元素间以指定的分隔符sep隔开。

- ##### list.insert(list, [pos=-1,] value)
在指定的pos位置后插入值为value的元素。

- ##### list.remove(list [, pos=-1])
删除pos位置的元素，并返回删除的值，若指定位置元素不存在，则返回nil。

- ##### list.slice(list [, start=1 [, end=-1 [, step=1]]])
对list进行切片，从start位置开始，end位置结束，步长为step（类似于python中的list[start : end : step]）。

- ##### list.pack(...)
将一系列元素打包成list。

- ##### list.unpack(list)
返回list中的所有元素。

## 其他库函数
- ##### locals()
获取局部变量，将其保存到table中。

- ##### generate(func)
将函数转换为生成器，生成器中可以使用yield中断并返回值。

```lua
> f = generate(function (a)
>>     for i = 1, a do
>>         yield(i, i * 2)
>>     end
>> end)
>
> for i, j in f(5) do
>>     print(i, j)
>> end
1       2
2       4
3       6
4       8
5       10
```

- ##### iter(it)
将可迭代对象转换成迭代器，可以是table, list, string或者其他迭代器。

```lua
> for i in iter([1, 2, 3]) do
>>      print(i)
>> end
1
2
3
```

```lua
> for i in iter("abc") do
>>      print(i)
>> end
a
b
c
```

- ##### enumerate(func)
将可迭代对象转换成带顺序索引的迭代器，索引计数从1开始。

```lua
> for i, v in enumerate([1, 2, 3]) do
>>      print(i, v)
>> end
1       1
2       2
3       3
```

```lua
> for i, v in enumerate("abc") do
>>      print(i, v)
>> end
1       a
2       b
3       c
```
