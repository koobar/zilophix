# coding: UTF-8
# ZilophiX ビルドスクリプト
# このスクリプトを実行することで、Zilophix のすべてのプロジェクトを一括でビルドできます。
# gcc（mingw-w64のgccを想定）があれば動作します。また、COMPILERの中身を"clang"に
# 置き換えると、clangを使用してビルドすることができます。
# 
# Pythonは、少なくとも3.12.7以降をインストールすることを推奨します。あまり古いと動作しません。
# makeやcmakeがインストールされている必要はありません。
#
# 【実行方法】
# 端末で、python build.py を入力してください
# .pyファイルの関連付けを行っている場合、ダブルクリックでも実行可能です。
#
# 【ビルド結果の出力先】
# オブジェクトファイルや実行可能ファイルは、以下の場所に生成されます。
#
#       ../bin          実行可能ファイル
#       ../obj          オブジェクトファイル

import sys
import subprocess
import glob
import os

compiler = "clang"                  # 使用するコンパイラ
global_options = "-O3"              # 全プロジェクト共通のオプション

# 指定されたディレクトリから数えて、cnt個前のディレクトリを取得する。
def get_previous_directory(source_path, cnt):
    # パスを区切り文字で分割し、スライスでcnt個前まで取得
    separated = source_path.split(os.sep)
    result = os.sep.join(separated[:-cnt])
    return result

# 指定されたパスのディレクトリが存在しなければ作成する。
def mkdir(folder_path):
    if (os.path.exists(folder_path) == False):
        os.mkdir(folder_path)

# コマンドを実行する
def execute_command(cmd):
    print(f">>{cmd}")
    subprocess.run(cmd, shell=True)

# プロジェクトをコンパイルする
def compile_project(project_dir, output_dir, options, include_list):
    os.makedirs(output_dir, exist_ok=True)  # ディレクトリが存在しない場合に作成

    # インクルードオプションを生成
    include_options = " ".join(f'-I "{include_file}"' for include_file in include_list or [])

    # ソースファイルをコンパイル
    for source_file in glob.glob(os.path.join(project_dir, "*.c")):
        base_name = os.path.splitext(os.path.basename(source_file))[0]
        
        # コマンドを生成し実行
        compile_command = (
            f"{compiler} {options} {global_options} -c \"{source_file}\" "
            f"-o \"{os.path.join(output_dir, base_name + '.o')}\" {include_options}"
        )
        execute_command(compile_command)

# link_dirsに指定されたディレクトリに存在する.oファイルをすべてリンクし、out_pathに指定されたファイルに保存する。
def link_files(output_path, additional_options, link_dirs):
    # オブジェクトファイルのパスを収集
    links = " ".join(
        f'"{file}"' 
        for dirname in (link_dirs or []) 
        for file in glob.glob(os.path.join(dirname, "*.o"))
    )
    
    # リンクコマンドを生成して実行
    link_command = f"{compiler} {links} -o \"{output_path}\" {additional_options}"
    execute_command(link_command)

# link_dirsに指定されたディレクトリに存在する.oファイルをすべてリンクし、実行可能ファイルを生成する。
def make_exec(proj_name, out_dir, link_dirs):
    link_files(f"{out_dir}{os.sep}{proj_name}.exe", "", link_dirs)

# link_dirsに指定されたディレクトリに存在する.oファイルをすべてリンクし、共有ライブラリファイルを生成する。
def make_shared_lib(proj_name, out_dir, link_dirs):
    link_files(f"{out_dir}{os.sep}{proj_name}.dll", "-shared", link_dirs)

# すべてのプロジェクトをビルドする
def build():
    src_dir = os.path.dirname(__file__)                             # build.py（このスクリプト）の場所
    libzilophix_dir = f"{src_dir}{os.sep}libzilophix"               # libzilophixのソースファイルの配置場所
    zilophixdll_dir = f"{src_dir}{os.sep}zilophixdll"               # zilophixdllのソースファイルの配置場所
    libwavefile_dir = f"{src_dir}{os.sep}libwavefile"               # libwavefileのソースファイルの配置場所
    zilophix_dir = f"{src_dir}{os.sep}zilophix"                     # zilophixのソースファイルの配置場所

    project_dir = get_previous_directory(src_dir, 1)                # プロジェクトの配置場所
    object_dir = f"{project_dir}{os.sep}obj"                        # オブジェクトファイル(.oファイル)の配置場所の親ディレクトリ
    libzilophix_obj_path = f"{object_dir}{os.sep}libzilophix"       # libzilophixのオブジェクトファイルの配置場所
    zilophixdll_obj_path = f"{object_dir}{os.sep}zilophixdll"       # zilophixdllのオブジェクトファイルの配置場所
    libwavefile_obj_path = f"{object_dir}{os.sep}libwavefile"       # libwavefileのオブジェクトファイルの配置場所
    zilophix_obj_path = f"{object_dir}{os.sep}zilophix"             # zilophixのオブジェクトファイルの配置場所

    bin_dir = f"{project_dir}{os.sep}bin"                           # 実行ファイルの配置場所

    # 古い出力ファイルがあれば削除し、出力用ディレクトリを作り直す。
    mkdir(object_dir)
    mkdir(bin_dir)

    # libzilophixをコンパイル
    compile_project(libzilophix_dir, libzilophix_obj_path, "-std=c99", None)

    # libwavefileをコンパイル
    compile_project(libwavefile_dir, libwavefile_obj_path, "-std=c99", None)

    # zilophixをコンパイル
    compile_project(zilophix_dir, zilophix_obj_path, "-std=c99", [f"{libzilophix_dir}{os.sep}include", f"{libwavefile_dir}{os.sep}include"])
    make_exec("zilophix", bin_dir, [libzilophix_obj_path, libwavefile_obj_path, zilophix_obj_path])

    # DLL版Zilophixのコンパイル
    compile_project(zilophixdll_dir, zilophixdll_obj_path, "-std=c99", [f"{libzilophix_dir}{os.sep}include", f"{libwavefile_dir}{os.sep}include"])
    make_shared_lib("zilophixdll", bin_dir, [libzilophix_obj_path, zilophixdll_obj_path, libwavefile_obj_path])

# コマンドライン引数を取得
# 1個目の引数は、スクリプトのファイル名が格納されることに注意。
args = sys.argv

# コマンドライン引数が2個以上指定されていれば、1個目をコンパイラ名として受け取る。
if (len(args) >= 2):
    compiler = args[1]

# コマンドライン引数が3個以上指定されていれば、2個目を最適化オプションとして受け取る。
if (len(args) >= 3):
    global_options = args[2]

print(f"Zilophix のコンパイルを開始します。コンパイラは {compiler} が、最適化オプションは {global_options} が使用されます。")
build()
print("コンパイルが終了しました。")