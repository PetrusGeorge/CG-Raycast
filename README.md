# Implementação do <i>raycasting</i> para a disciplina de Computação Gráfica

Um simples programa que visa mostrar os conhecimentos adquiridos na disciplina a partir da implementação do algoritmo de <i>raycasting</i> para renderização de cenas 3D.

---

## O que o código faz
- O código é uma interface para geração de cenas utilizando o algoritmo <i>raycasting</i> de renderização 3D.
- O usuário deve descrever o conjunto de elementos da cena (luz e objetos) a nível de código.
- A cena será renderizada com efeitos de oclusão e sombra intrísecos ao algoritmo implementado.

---

## Imagens do programa
![Imagem do Programa](./images/cubes.png)

*Exemplo de cena: Cubes*

![Imagem do Programa](./images/towers.png)

*Exemplo de cena: Towers*

![Imagem do Programa](./images/walls.png)

*Exemplo de cena: Walls*

---

## Compilação e Execução

### Dependências
- C++ 17 (gcc)
- GLUT
- OpenGL

### Compilação
1. Baixe/clone este repositório.
2. No terminal, navegue até a pasta do projeto.
3. Execute:
```bash
make
```
### Execução
---

## Principais problemas encontrados
- Tentamos inicialmente utilizar funções nativas do OpenGL, mas claramente não deu certo.
- Decidimos fazer o algoritmo renderizador com as primitivas sendo triângulos, dessa forma, objetos simples como esfera ficaram muito pesados e complexos de representar.
- As cores dos objetos estavam incoerentes, pois era necessário que o produto interno do vetor normal ao objeto com o vetor que vai para a luz deve sempre ser positivo

---

## 🔧 O que pode ser melhorado
- É possível implementar uma estrutura de dados que diminuem as verificações de interseção para acelerar o código.

---

## 👥 Contribuição da Equipe
Foi feita o desenvolvimento em conjunto onde uma pessoa mexia no código e as outras duas auxiliavam externamente, dessa forma, todos os integrantes fizeram tudo juntos.
