import json
import matplotlib.pyplot as plt
import numpy as np


def media_aritmetica():
    print(1)
    
def desvio_padrao(dados):
    times = [r["time"][0] for r in dados[0]["result"]]

    # Calcular o desvio padrão
    std_deviation = np.std(times)

    # Criar o gráfico de barras
    threads = [r["threads"] for r in dados[0]["result"]]
    plt.bar(threads, [std_deviation] * len(threads), color='b', alpha=0.5, label='Desvio Padrão')
    plt.title('Desvio Padrão dos Tempos para Cada Número de Threads')
    plt.xlabel('Número de Threads')
    plt.ylabel('Desvio Padrão')
    plt.legend()
    plt.grid(True)
    plt.show()
    
def grafico_speedup(dados):
    speedup = list()
    for conjunto in dados:
        threads = [r["threads"] for r in conjunto["result"]]
        times = [r["time"][0] for r in conjunto["result"]]
        base_time = times[0]  # Tempo com 1 thread

        # Calcular o speedup
        speedup = ([base_time / t for t in times])

        # Criar o gráfico
        plt.figure(figsize=(8, 6))
        plt.plot(threads, speedup, marker='o', linestyle='-')
        plt.title('Gráfico de Speedup')
        plt.xlabel('Número de Threads')
        plt.ylabel('Speedup')
        plt.grid(True)
        
        plt.savefig(f'result/speedUp in Cell({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        plt.clf()

def grafico_eficiencia():
    print(1)

def grafico_thread_tempo(dados):
    for conjunto in dados:
        threads = [item["threads"] for item in conjunto["result"]]
        tempos = [item["time"] for item in conjunto["result"]]


        matriz_invertida = []
        for i in range(len(tempos[0])):
            linha = []
            for j in range(len(tempos)):
                linha.append(tempos[j][i])
            matriz_invertida.append(linha)

        for i in matriz_invertida:
            cor = plt.cm.viridis(float(matriz_invertida.index(i)) / len(matriz_invertida))
            legenda = f'tentativa {matriz_invertida.index(i)}°'
            plt.plot(threads, i, marker='o', linestyle='-', label=legenda, color=cor)

        #plt.plot(threads, tempos, marker='o', linestyle='-', label=f'Cell {conjunto["cell_height"]}x{conjunto["cell_width"]}', color=cor)

        plt.xlabel('Número de Threads')
        plt.ylabel('Tempo (segundos)')
        plt.title(f'Número de Threads vs Tempo em Célula({conjunto["cell_height"]}x{conjunto["cell_width"]})')
        plt.grid(True)
        plt.ticklabel_format(style='plain', axis='y')
        plt.xticks(range(0, max(threads) + 1))
        plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), title="Legendas")

        plt.savefig(f'result/thread x time in Cell({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        plt.clf()

def grafico_dispercao(dados):
    for conjunto in dados:
        threads = [item["threads"] for item in conjunto["result"]]
        tempos = [item["time"] for item in conjunto["result"]]

        index_thread = 0
        for tempo in tempos:
            thread_copy = [threads[index_thread]] * len(tempo)
            plt.scatter(x=thread_copy, y=tempo, marker='.')
            index_thread += 1
            
            
        plt.xlabel('Número de Threads')
        plt.ylabel('Tempo (segundos)')
        plt.title(f'Gráfico de disperção Threads vs Tempo({conjunto["cell_height"]}x{conjunto["cell_width"]})')
        plt.savefig(f'result/disperção thread x time in Cell({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        plt.clf()
        
        
        
with open('result/dados.json', 'r') as arquivo_json:
    dados = json.load(arquivo_json)

    grafico_thread_tempo(dados)
    grafico_dispercao(dados)
    #desvio_padrao(dados)
    grafico_speedup(dados)
