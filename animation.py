from PIL import Image
import os

# Diretório que contém as imagens PGM
directory = "result/animation/"

# Obtenha a lista de nomes de arquivos das imagens PGM
image_files = [f for f in os.listdir(directory) if f.endswith(".pgm")]

# Abra as imagens e adicione-as a uma lista
images = []
for file_name in image_files:
    image = Image.open(os.path.join(directory, file_name))
    images.append(image)

# Salve as imagens como um arquivo GIF
output_file = "animation.gif"
images[0].save(output_file, save_all=True, append_images=images[1:], loop=0, duration=500)

print(f"O GIF foi criado e salvo como {output_file}")
