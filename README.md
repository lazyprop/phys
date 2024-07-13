# physics engine

the total mechanical energy of the system ~is~ was changing. [euler integration](https://en.wikipedia.org/wiki/Euler_method)
has an [energy drift](https://en.wikipedia.org/wiki/Energy_drift) problem for
second order systems.


i've switched to verlet now. two body orbits are mostly stable. total energy fluctuates,
especially at high velocities, but is stable over time.

![energy.png]

- [integration basics by gafer on games](https://gafferongames.com/post/integration_basics/)
- https://adamsturge.github.io/Engine-Blog/mydoc_symplectic_euler.html#symplectic-euler

