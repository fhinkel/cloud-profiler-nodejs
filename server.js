const profiler = require('./out/src/index')

function inner(obj, count = 1){
    let res = 0;
    for(let i = 0; i < count; i++) {
        res += obj.x;
    }
    return res;
}

function outer() {
    const upper_bound = 10000;
    let total = 0;

    let objects = [
        {x: 1},
        {y: 12, x: 1},
        {x: 1.0},
        {y: 2, z: 4, x: 1},
        {a: 2, z: 4, x: 1},
        {b: 2, z: 4, x: 1}
    ];

    let obj = objects[0];

    let res = 0;
    for(let i = 0; i < upper_bound; i++) {
        res += obj.x;
    }

    let sum = inner({b: 2, z: 4, x: 1}, upper_bound);

    total += sum + res;

    setImmediate(outer);
}



profiler.startLocal({
  projectId: 'nolanmar-testappstandard',
  logLevel: 5,
  serviceContext: {
    service: 'app-std',
    version: '0'
  },
});

outer();