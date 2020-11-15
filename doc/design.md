## 词法分析

###1. 词法分类

classify

``` 
Identifier
Keyword
Punctuator
IntLiterial
FloatLiterial
DoubleLiterial
CharLiterial
StringLiterial
```
keyword

```
void _Bool char short int long float double struct enum sizeof
static typedef
if else switch case default
do while for break continue goto
return 
```
Punctuator

``` 
() [] . -> ++ --
! & ~
* / % 
+ - 
<< >>
< <= > >= 
== != 
& ^ |
&& ||
? :
= += -= *= /= %= <<= >>= &= |= ^=
,
``` 
### 2, 状态机

状态 

```
Start, Identifier, Int, Double, Char, String, 
Punctuation, SingleComment, MultiComment
``` 

辅助函数   

```   
bool IsLetter(char ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool IsDigit(char ch) {
	return ch >= '0' && ch <= '9';
}

bool IsPunctuation(char ch) {
	return ch == '{' || ....
}
``` 

### 3，解析5类单词  

1, 标识符 （关键字）   
2, 数值常量 (int, float)   
3, 字符常量   
4, 字符串常量   
5, 操作符 (标点符号)



