# Engenheiro Eletronico LLM - Llama 3.2 3B

Este repositório contém o ecossistema completo para realizar a fusão 
(merge), conversão e execução de um modelo Llama 3.2 3B finetunado para 
engenharia eletrônica. O projeto utiliza adaptadores LoRA treinados para responder com raciocínio lógico estruturado.

---

## SUMÁRIO

1. [Pré-requisitos](#pré-requisitos)
2. [Passo 1: Obtenção dos Adaptadores (Colab)](#passo-1-obtenção-dos-adaptadores-colab)
3. [Passo 2: Instalação de Dependências](#passo-2-instalação-de-dependências)
4. [Passo 3: Fusão do Modelo (Merge)](#passo-3-fusão-do-modelo-merge)
5. [Passo 4: Conversão para GGUF](#passo-4-conversão-para-gguf)
6. [Passo 5: Execução do Chat](#passo-5-execução-do-chat)

---

## Pré-requisitos

- python: 3.10+
- espaço em disco: aproximadamente 15GB livres.
- hardware: Recomenda-se 16GB+ de RAM para o processo de merge. Uma GPU NVIDIA é recomendada para o char, mas o modelo pode rode em CPU.

---

## Passo a Passo

Passo 1: Obtenção dos Adaptadores (Colab)
Os pesos do modelo (safetensors) não estão incluídos neste repositório devido ao tamanho. Para obtê-los:

Abra o arquivo notebook (.ipynb) presente neste repositório no Google Colab.

Execute o treinamento conforme as instruções do notebook.

Após o término, baixe a pasta contendo os adaptadores (arquivos como adapter_model.safetensors e adapter_config.json) para o seu computador local.

Passo 2: Instalação de Dependências
No seu terminal (Git Bash recomendado no Windows), instale as bibliotecas necessárias:

```bash
pip install -r requirements.txt
```
Passo 3: Fusão do Modelo (Merge)
Antes de converter para GGUF, é necessário unir os adaptadores LoRA ao modelo base.

Abra o arquivo converter.py (referenciado como converter.txt nos scripts originais).

Ajuste a variável CAMINHO_ADAPTADORES para a pasta onde você salvou os arquivos do Colab.

Execute o script:

```bash
python converter.txt
```
O script carregará o modelo base unsloth/Llama-3.2-3B-Instruct na CPU para economizar VRAM e salvará o modelo completo na pasta modelo_pronto_16bit.

Passo 4: Conversão para GGUF
Agora, transforme o modelo fundido em um único arquivo .gguf, otimizado para inferência local:

```bash
python convert_hf_to_gguf.py modelo_pronto_16bit --outfile engenheiro.gguf --outtype f16
```
Este comando utiliza o script convert_hf_to_gguf.py para gerar o arquivo engenheiro.gguf.

Passo 5: Execução do Chat
Com o arquivo engenheiro.gguf na mesma pasta, inicie o chat interativo:

```bash
python chat.py
```

Nota: Caso utilize GPU NVIDIA, instale o llama-cpp-python com suporte CUDA para melhor desempenho no chat.


