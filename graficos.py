import json
import matplotlib.pyplot as plt

with open('result/dados.json', 'r') as arquivo_json:
    dados = json.load(arquivo_json)

for conjunto in dados:
    threads = [item["threads"] for item in conjunto["result"]]
    tempo = [item["time"] for item in conjunto["result"]]

    cor = plt.cm.viridis(float(dados.index(conjunto)) / len(dados))

    plt.plot(threads, tempo, marker='o', linestyle='-', label=f'Cell {conjunto["cell_height"]}x{conjunto["cell_width"]}', color=cor)


plt.xlabel('Número de Threads')
plt.ylabel('Tempo (segundos)')
plt.title('Tempo vs. Número de Threads')
plt.grid(True)
plt.ticklabel_format(style='plain', axis='y')
plt.xticks(range(1, max(threads) + 1))
plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), title="Legendas")

plt.savefig("result/output.png", bbox_inches="tight")

