# /bin/bash
jupyter nbconvert --to latex $1
ipynb_path=$1
echo ${ipynb_path}
name_len=${#ipynb_path}
echo ${name_len}
name=${ipynb_path:0:name_len-6}
tex_path="${ipynb_path:0:name_len-6}".tex""
echo ${tex_path}
search_string="{article}"  # 替换为要查找的字符串
text_to_append="    \usepackage{fontspec, xunicode, xltxtra}
	\setmainfont{Ubuntu-R.ttf}
	\usepackage{ctex}"  # 替换为要添加的文字
awk -v search="$search_string" -v text="$text_to_append" '{
    print
    if ($0 ~ search && !added) {
        print text
        added = 1
    }
}' "$tex_path" > temp && mv temp "$tex_path"
xelatex ${tex_path}
rm "${name}".log""
rm "${name}".out""
rm "${name}".tex""
rm "${name}".aux""
rm -rf "${name}"_files""