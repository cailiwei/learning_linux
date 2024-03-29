## 常用命令

-------------------------------------------------------------------------------

### 1、!组合键

-------------------------------------------------------------------------------

```shell

 !! # 重复执行上一条指令

 !a # 重复执行上一条以a为首的指令
 !number # 重复执行上一条在history表中记录号码为number的指令

 !-number # 重复执行前第number条指令

 !$ # 表示获得上一条命令中的最后一项内容

```

-------------------------------------------------------------------------------

### 2、Ctrl组合键

-------------------------------------------------------------------------------

```cmd

Ctrl+r # 来进入历史搜索模式在history表中查询某条过往指令，找到需要重复执行的命令后，按回车键即可重复命令参数(即上一点中的第5条)

Ctrl+p # 重复上一次命令

Ctrl+a # 跳到第一个字符前

Ctrl+e # 转到字符尾部

Ctrl+x # 同上但再按一次会从新回到原位置

Ctrl+h # 删除前一个字符

Ctrl+u # 删除提示符前的所有字符

Ctrl+w # 删除光标前的一个单词

Ctrl+d # 删除提示符后一个字符或exit或logout

Ctrl+b # 前移一个字符不删除字符情况下

Ctrl+f # 后移一个字符

Ctrl+k # 删除提示符后全部字符 或 取消

Ctrl+i # 等同于按制表符<TAB>键

Ctrl+m # 等同于回车键

Ctrl+o # 等同于回车键

Ctrl+v # 使下一个特殊字符可以插入在当前位置, 如Ctrl+V <TAB> 可以在当前位置插入一个<TAB>字符, 其ASCII是9, 否则一般情况下按<TAB>结果是命令补齐

Ctrl+c # 撤消当前命令行的编辑, 另起一行.

Ctrl+s # 暂时冻结当前shell的输入

Ctrl+q # 解冻

Ctrl+t # 将光标处的字符和光标前一个字符替换位置

Ctrl+y # 粘贴或者恢复上次的删除

Ctrl+z # 把当前进程转到后台运行，使用' fg ‘命令恢复。比如top -d1 然后ctrl+z ，到后台，然后fg,重新恢复

```

-------------------------------------------------------------------------------

### 3、Esc组合

-------------------------------------------------------------------------------

```cmd

Esc+c # 使下一个单词首字母大写, 同时光标前进一个单词, 如光标停留在单词的某个字母上, 如word中的o字母上, 则o字母变大写, 而不是w

Esc+u # 使下一个单词所有字母变大写, 同时光标前进一个单词, 同上, 如光标在o字母上, 则ord变大写, w不变.

Esc+l # 同ESC-U, 但使之全变为小写.

Esc+d # 删除光标后的一个词

Esc+f # 往右跳一个词

Esc+b # 往左跳一个词

Esc+t # 交换光标位置前的两个单词。

Esc+f # 光标向前步进一个单词

Esc+b # 光标向后步进一个单词

```
