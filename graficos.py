import json
import matplotlib.pyplot as plt
import numpy as np
import csv

def calc_media(times):
    media_aritmetica = []
    for time in times:
        media_aritmetica.append(np.mean(time))
    return media_aritmetica

def media_aritmetica(dados):
    for conjunto in dados:
        threads = [r["threads"] for r in conjunto["result"]]
        times = [r["time"] for r in conjunto["result"]]

        media_aritmetica = calc_media(times)
            
        plt.plot(threads, media_aritmetica, marker='o', linestyle='-')
        plt.title('Média Aritmética')
        plt.xlabel('Número de Threads')
        plt.ylabel('Média')
        plt.grid(True)
        
        plt.savefig(f'result/Média({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        

def desvio_padrao(dados):
    for conjunto in dados:
        threads = [r["threads"] for r in conjunto["result"]]
        times = [r["time"] for r in conjunto["result"]]

        media_aritmetica = calc_media(times)
        desvio_padrao = []
        for time in times:
            desvio_padrao.append(np.std(time))
        
        plt.errorbar(x=threads, 
                     y=media_aritmetica, 
                     yerr=desvio_padrao,
                     ecolor='r',
                     linestyle='',
                     label="Desvio padrâo",
                     capsize=5, elinewidth=2, markeredgewidth=2,
                     zorder=1)
        plt.scatter(x=threads,
                y=media_aritmetica,
                label="Média Aritmética",
                zorder=2)

        plt.title('Desvio Padrão')
        plt.xlabel('Número de Threads')
        plt.ylabel('Média Aritmética')
        plt.grid(False)
        plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), title="Legendas")
        plt.savefig(f'result/Desvio padrão({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        
def calc_speedup(times):
    times_invert = list(map(list, zip(*times)))
    
    speedup = []
    for i, tempos in enumerate(times_invert):
        sequential_time = tempos[0]
        speedup.append([sequential_time / time for time in tempos])
    return speedup

def grafico_speedup(dados):
    for conjunto in dados:
        threads = [r["threads"] for r in conjunto["result"]]
        times = [r["time"] for r in conjunto["result"]]

        speedups = calc_speedup(times)

        for i, speedup in enumerate(speedups):
            legenda = f'Execução {i+1}°'
            plt.plot(threads, speedup, marker='o', linestyle='-', label=legenda)

        plt.title('Gráfico de Speedup')
        plt.xlabel('Número de Threads')
        plt.ylabel('Speedup')
        plt.grid(True)
        plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), title="Legendas")
        plt.savefig(f'result/SpeedUp({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        

def grafico_eficiencia(dados):
    for conjunto in dados:
        threads = [r["threads"] for r in conjunto["result"]]
        times = [r["time"] for r in conjunto["result"]]
        
        # Calcular o speedup
        speedups = calc_speedup(times)
        #
        for i, speedup in enumerate(speedups):
            efficiency = [mono_speedup / t for mono_speedup, t in zip(speedup, threads)]
            legenda = f'Execução {i+1}°'
            plt.plot(threads, efficiency, marker='o', linestyle='-', label=legenda)

        plt.title('Gráfico de Eficiência')
        plt.xlabel('Número de Threads')
        plt.ylabel('Eficiência')
        plt.grid(True)
        plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), title="Legendas")
        plt.savefig(f'result/Eficiência({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        

def grafico_thread_tempo(dados):
    for conjunto in dados:
        threads = [item["threads"] for item in conjunto["result"]]
        tempos = [item["time"] for item in conjunto["result"]]

        times_invert = list(map(list, zip(*tempos)))

        for i, time in enumerate(times_invert):
            legenda = f'Execução {i+1}°'
            plt.plot(threads, time, marker='o', linestyle='-', label=legenda)



        plt.title(f'Número de Threads vs Tempo em Célula({conjunto["cell_height"]}x{conjunto["cell_width"]})')
        plt.xlabel('Número de Threads')
        plt.ylabel('Tempo (segundos)')
        plt.grid(True)
        plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), title="Legendas")
        plt.savefig(f'result/Thread x time({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        

def grafico_dispercao(dados):
    for conjunto in dados:
        threads = [item["threads"] for item in conjunto["result"]]
        tempos = [item["time"] for item in conjunto["result"]]

        thread_copy = []
        for i, tempo in enumerate(tempos):
            thread_copy.append([threads[i]] * len(tempo))
        
        plt.scatter(y=thread_copy, x=tempos, marker='.')
        plt.ylabel('Número de Threads')
        plt.xlabel('Tempo (segundos)')
        plt.title(f'Gráfico de disperção Threads vs Tempo({conjunto["cell_height"]}x{conjunto["cell_width"]})')
        plt.savefig(f'result/Disperção thread x time({conjunto["cell_height"]}x{conjunto["cell_width"]}).png', bbox_inches="tight")
        
        
def csv_data(dados):
    with open('output.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["cell_height", "cell_width", "threads", "time"])

        # Iterate through the JSON data and write to CSV
        for item in dados:
            cell_height = item["cell_height"]
            cell_width = item["cell_width"]
            for result in item["result"]:
                threads = result["threads"]
                for time in result["time"]:
                    writer.writerow([cell_height, cell_width, threads, time])


with open('result/dados.json', 'r') as arquivo_json:
    dados = json.load(arquivo_json)
    
    grafico_eficiencia(dados)
    plt.clf()
    grafico_thread_tempo(dados)
    plt.clf()
    grafico_dispercao(dados)
    plt.clf()
    desvio_padrao(dados)
    plt.clf()
    media_aritmetica(dados)
    plt.clf()
    grafico_speedup(dados)
    plt.clf()
    

    
