# lua-list
在不改动lua的其他语法功能的情况下，在lua中添加链表类型的支持，基于lua5.4.0-rc3。

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

- 按索引遍历的时间复杂度为O(n)

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