const profiler = require('./out/src/index')

function inner(obj, count = 1){
    let res = 0;
    for(let i = 0; i < count; i++) {
        res += obj.x;
    }
    return res;
}

function middle1(obj, count) {
    // and
    // here
    // is some 
    // line number.
    inner(obj, count);
}

function middle2(obj, count) {
    inner(obj, count);
}

function outer() {
    let sum = 0;
    const upper_bound = 10000;

    let obj = {x: 1};
    let res = 0;

    sum += middle1({b: 2, z: 4, x: 1}, upper_bound);

    if (false) {
        sum += middle1({b: 2, z: 4, x: 1}, upper_bound);
    }

    if (true) {
        sum += middle1({b: 2, z: 4, x: 1}, upper_bound);
        sum += middle2({b: 2, z: 4, x: 1}, upper_bound);
    }

    for(let i = 0; i < upper_bound; i++) {
        res += obj.x;
    }


    let total = sum + res;
    if (total === 17) {
        console.log(total);
    }
    // setTimeout(outer, 0);
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