import sqlite3
import pandas as pd
import matplotlib.pyplot as plt

print("正在连接 SQLite 数据库...")

# 1. 连接 C++ 生成的数据库文件
# 注意：这里假设你的 db 文件生成在 build 文件夹下
db_path = 'build/srd_data.db' 
conn = sqlite3.connect(db_path)

# 2. 用 SQL 语句把数据提取出来，并直接转化为 Pandas 数据框 (DataFrame)
# 我们筛选出转速大于 0 的有效运行数据
query = "SELECT time_ms, target_rpm, current_a FROM motor_log WHERE target_rpm > 0"
df = pd.read_sql_query(query, conn)

# 关闭数据库连接
conn.close()

# 3. 打印一份专业的数据统计报告
print("\n========== 深蓝动力 SRD 电机运行报告 ==========")
print(f"共采集到有效运行数据: {len(df)} 条")
if len(df) > 0:
    print(f"最大峰值电流: {df['current_a'].max():.2f} A")
    print(f"最低波谷电流: {df['current_a'].min():.2f} A")
    print(f"平均运行电流: {df['current_a'].mean():.2f} A")
    print("===============================================\n")

    # 4. 生成数据可视化图表
    print("正在生成动力学分析图表...")
    plt.figure(figsize=(10, 5)) # 设置画布大小
    
    # 画出时间-电流曲线
    plt.plot(df['time_ms'], df['current_a'], label='Phase A Real-time Current', color='#FF5733', linewidth=1.5)
    
    # 设置图表的标题和坐标轴
    plt.title('SRD Motor Dynamics Analysis (Python Powered)', fontsize=14, fontweight='bold')
    plt.xlabel('Time (ms)', fontsize=12)
    plt.ylabel('Current (A)', fontsize=12)
    
    plt.grid(True, linestyle='--', alpha=0.6) # 添加网格线
    plt.legend() # 显示图例
    
    # 弹出窗口展示图表
    plt.show()
else:
    print("数据库中没有转速大于 0 的有效数据，请先运行 C++ 软件生成一些数据！")