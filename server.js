const profiler = require('./out/src/index')

function inner(obj, count = 1){
    let res = 0;
    for(let i = 0; i < count; i++) {
        res += obj.x;
    }
    return res;
}

function middle1(obj, count) {
    inner(obj, count);
}

function middle2(obj, count) {
    inner(obj, count);
}

function outer() {
    let sum = 0;
    const upper_bound = 10000;

    if (false) {
        sum += middle1({b: 2, z: 4, x: 1}, upper_bound);
    }

    let obj = {x: 1};
    let res = 0;
    for(let i = 0; i < upper_bound; i++) {
        res += obj.x;
    }

    if (true) {
        sum += middle1({b: 2, z: 4, x: 1}, upper_bound);
        sum += middle2({b: 2, z: 4, x: 1}, upper_bound);
    }

    let total = sum + res;
    if (total === 17) {
        console.log(total);
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