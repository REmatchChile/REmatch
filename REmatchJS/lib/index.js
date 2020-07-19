// Importación de REmatch
const Module = require('./rematch_wasm.js');

// const { RegEx, RegExOptions } = Module;
// let match;
// let rgxOptions = new RegExOptions();
// rgxOptions.early_output = true;
// let rgx = new RegEx('.*!x{a}.*', rgxOptions);
// console.log('Matches:')
// while(match = rgx.findIter('aaaa')) {
//   console.log(match.span('x'));
// }


const { RegEx, RegExOptions } = Module;

let early_output;
let line_by_line;
let multi_line;
let dot_nl;
let save_anchors;

/* Regex object */
class Regex {
  constructor(pattern, options) {
    this.pattern = pattern
    this.rgx_object = new RegEx(pattern, options)
    this.options = options
  }
  find(string) {
    return new Match(this.rgx_object.find(string));
  }

  *findIter(string) {
    let match;
    while (match = this.rgx_object.findIter(string)) {
      match = new Match(match)
      if (match.obj == null) {
        return null
      }
      yield match
    }
  }
  match(string) {
    throw new Error('Method not available')
    this.options.save_anchors = true
    let rgx = new RegEx(this.pattern, this.options);
    let m = new Match(rgx.find(string));
    if (m.group(0) == string.slice(0, m.group(0).length + 1)) {
      return m
    }

  }
  search(string) {
    return new Match(this.rgx_object.find(string));
  }
  fullmatch(string) {
    throw new Error('Method not available')
    this.options.save_anchors = true
    let rgx = new RegEx(this.pattern, this.options);
    let m = new Match(rgx.find(string));
    if (m.group(0) == string) {
      return m
    }
  }
  findall(string) {
    let vector = [];
    let match;
    while (match = this.rgx_object.findIter(string)) {
      match = new Match(match)
      vector.push(match);
    }
    return vector
  }
}

/* Match object */
class Match {
  constructor(obj) {
    this.obj = obj
  }
  convert(varb) {
    if (varb.isInteger) {
      throw new Error('Access by variable index is not available')
      // varb = varb - 1; // si?
      return this.obj.variables()[varb];
    }
    return varb
  }
  start(varb) {
    varb = this.convert(varb);
    return this.obj.start(varb)
  }
  end(varb) {
    varb = this.convert(varb);
    return this.obj.end(varb)
  }
  span(varb) {
    varb = this.convert(varb);
    return this.obj.span(varb);
  }
  group(varb) {
    varb = this.convert(varb);
    return this.obj.group(varb)
  }
  groups() {
    throw new Error('Method not available')
    let content = [];
    let vars = this.obj.variables();
    for (let i = 0; i < vars.length; i++) {
      const varb = vars[i];
      content.push(this.obj.group(varb))
    }
    return content
  }
  groupdict() {
    throw new Error('Method not available')
    let dic = {};
    let vars = this.obj.variables();
    for (let i = 0; i < vars.length; i++) {
      const varb = vars[i];
      dic[varb] = this.obj.group(varb);
    }
    return dic
  }

}

/* Compile */
const compile = function (pattern, ...flags) {
  let rgxOptions = new RegExOptions();
  if (multi_line) {
    console.warn('Flags are not available')
    rgxOptions.multi_line = true;
  }
  if (early_output) {
    console.warn('Flags are not available')
    rgxOptions.early_output = true;
  }
  if (line_by_line) {
    console.warn('Flags are not available')
    rgxOptions.line_by_line = true;
  }
  if (dot_nl) {
    console.warn('Flags are not available')
    rgxOptions.dot_nl = true;
  }
  if (save_anchors) {
    console.warn('Flags are not available')
    rgxOptions.save_anchors = true;
  }

  let rgx = new Regex(pattern);
  return rgx
}

/* Direct methods */
const find = function (pattern, string, ...flags) {
  let rgx = compile(pattern, ...flags);
  return rgx.find(string)
}
const search = function (pattern, string, ...flags) {
  let rgx = compile(pattern, ...flags);
  return rgx.search(string)
}
const findall = function (pattern, string, ...flags) {
  let rgx = compile(pattern, ...flags);
  return rgx.findall(string)
}
const findIter = function (pattern, string, ...flags) {
  let rgx = compile(pattern, ...flags);
  return rgx.findIter(string)
}
const match = function (pattern, string, ...flags) {
  let rgx = compile(pattern, ...flags);
  return rgx.match(string)
}
const fullmatch = function (pattern, string, ...flags) {
  let rgx = compile(pattern, ...flags);
  return rgx.fullmatch(string)
}
// archivos grandes: find no funciona
// username en correos de más de 4 caracteres sale output raro, caracteres extraños
module.exports = {
  compile: compile,
  match: match,
  fullmatch: fullmatch,
  findIter: findIter,
  findall: findall,
  find: find,
  search: search
}