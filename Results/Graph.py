import pandas as pd
import matplotlib.pyplot as plt

file_config = {
    'Naive.csv': {
        'color': 'red',
        'label': 'Naive',
        'line_style': '-'
    },
    'Array.csv': {
        'color': 'blue',
        'label': 'Array',
        'line_style': '-'
    },
    'AVX.csv': {
        'color': 'green',
        'label': 'AVX',
        'line_style': '-'
    }
}

plt.figure(figsize=(12, 6))
plt.xlabel('Номер серии', fontsize=12)
plt.ylabel('Время (с)', fontsize=12)
plt.title('Сравнение способов подсчёта мн-ва Мандельброта', fontsize=14, pad=15)
plt.grid(True, linestyle=':', alpha=0.7)

for file, config in file_config.items():
    try:
        df = pd.read_csv(file).sort_values('num')
        plt.plot(
            df['num'],
            df['time_avg_s'],
            color=config['color'],
            linestyle=config['line_style'],
            linewidth=2,
            markersize=8,
            markevery=2,
            label=config['label']
        )
    except Exception as e:
        print(f'Ошибка: {str(e)}')
        continue

plt.legend(
    title='Алгоритмы:',
    loc='upper left',
    bbox_to_anchor=(1, 1),
    fontsize=10,
    title_fontsize=11
)

plt.xticks(range(0, 8, 1), fontsize=10)
plt.yticks(fontsize=10)
plt.xlim(0, 8)
plt.ylim(0, 0.1)

plt.tight_layout()
plt.show()