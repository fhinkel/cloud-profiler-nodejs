const profiler = require('./out/src/index')

function inner(obj){
  return obj.x;
}

function outer() {
  const upper_bound = 10000;
  let sum = 0;

  for (let i = 0; i < upper_bound; i++) {
    sum += inner({x: 1});
    sum += inner({y: 12, x: 1});
    sum += inner({x: 1.0});
    sum += inner({y: 2, z: 4, x: 1});
    sum += inner({a: 2, z: 4, x: 1});
    sum += inner({b: 2, z: 4, x: 1});

    let objects = [
        {x: 1},
        {y: 12, x: 1},
        {x: 1.0},
        {y: 2, z: 4, x: 1},
        {a: 2, z: 4, x: 1},
        {b: 2, z: 4, x: 1}
    ];

    for (let obj of objects) {
        sum += obj.x;
    }

    let obj = {x: 1};
    sum += obj.x;

    obj =  {y: 12, x: 1};
    sum += obj.x;

    obj = {x: 1.0};
    sum += obj.x;

    obj = {y: 2, z: 4, x: 1};
    sum += obj.x;

    obj = {a: 2, z: 4, x: 1};
    sum += obj.x;

    obj = {b: 2, z: 4, x: 1};
    sum += obj.x;

  }
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