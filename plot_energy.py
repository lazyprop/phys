import matplotlib.pyplot as plt

with open('energies.txt', 'r') as f:
    lines = list(filter(lambda x: not x.startswith('INFO'), f.readlines()))
    energies = list(map(float, lines))

plt.plot(energies)
plt.show()
