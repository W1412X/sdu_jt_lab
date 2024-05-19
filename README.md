# 山东大学计算机科学与技术学院计算机图形学实验代码
# 2024.3
# 实验仅供参考，不要直接复制
## 写一个小问题
### 使用`pyenv`管理全局环境时利用`virtualenv`创建虚拟环境出现`No module named 'pip._vendor.six'`问题
- 这个问题是`pip`自身的问题，只需要重装pip就可以了(具体为什么是pip的问题我也不知道，社区上这么写的)
- 解决办法 - 重装pip (**注意是在你的虚拟环境执行如下操作**)
  - 获取`get-pip.py`文件
  ```shell
  curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
  ```
  - 强制重装
  ```shell
  python get-pip.py --force-reinstall
  ```
