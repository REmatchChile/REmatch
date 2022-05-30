// Importación de REmatch
const Module = require('../../bin/rematch_wasm.js');

Module.onRuntimeInitialized = () => {
  // Unpack de clases
  const {RegEx, RegExOptions} = Module;

  let match;
  let rgxOptions = new RegExOptions();
  rgxOptions.early_output = false;
  rgxOptions.line_by_line = true;

  let rgx = new RegEx('.*!x{a*}.*', rgxOptions);

  let count = 0;
  while (match = rgx.findIter('aaaabbbbabaaaabababababasdasdskjanacs', 0)) {
    count += 1;
  }
}
