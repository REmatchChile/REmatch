
// Importación de REmatch
const Module = require('../../embuild/bin/rematch_wasm.js');

// Unpack de clases
const {RegEx, RegExOptions} = Module;


let match;
let rgxOptions = new RegExOptions();
rgxOptions.early_output = false;
rgxOptions.line_by_line = false;

let rgx = new RegEx('.*!x{a+}.*', rgxOptions);

let count = 0;

rgx.feed('a')


while(match = rgx.internalFindIter()) {
    count += 1
}
console.log(count)

