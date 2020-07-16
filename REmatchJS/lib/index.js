const hello = function (){
  return 'Hello world!'
}
export {hello}

// Importación de REmatch
const Module = require('../../REmatchEngine/bin/rematch_wasm.js');
const { match } = require('assert');

Module.onRuntimeInitialized = () => {
    // Unpack de clases
    const {RegEx, RegExOptions} = Module;

    // let match;
    // let rgxOptions = new RegExOptions();
    // rgxOptions.early_output = false;
    // rgxOptions.line_by_line = true;

    // let rgx = new RegEx('.*!x{a}.*', rgxOptions);

    // let count = 0;
    // while(match = rgx.findIter('aaaa')) {
    //     count += 1;
    // }
    // console.log(count)
    //////////////////////////////////////////////////////
    
    try {
      
      let early_output;
      let line_by_line;
      let multi_line;
      let dot_nl;
      let save_anchors;
      /*   Regex object */
      class Regex {
        constructor(pattern, options) {
          this.pattern = pattern
          this.rgx_object =  new RegEx(pattern, options)
          this.options = options
        }
        find (string) {
          return new Match(this.rgx_object.find(string));
        }
        
        *findIter (string) {
            let match;
            while (match = this.rgx_object.findIter(string)) {
              match = new Match(match)
              yield match
              
            }
          }
        match (string) {
          this.options.save_anchors = true
          let rgx = new RegEx(this.pattern, this.options);
          let m = new Match(rgx.find(string));
          if (m.group(0) == string.slice(0, m.group(0).length + 1)) {
            return m
          }

        }
        search (string) {
          return new Match(this.rgx_object.find(string));
        }
        fullmatch (string) {
          this.options.save_anchors = true
          let rgx = new RegEx(this.pattern, this.options);
          let m = new Match(rgx.find(string));
          if (m.group(0) == string) {
            return m
          }
        }
        findall (string) {
          let vector = [];
          let match;
          while(match = this.rgx_object.findIter(string)) {
            match = new Match(match)
            vector.push(match);
          }
          return vector
        }
      }
      class Match {
        constructor (obj) {
          this.obj = obj
        }
        convert (varb) {
          if (varb.isInteger) {
            // varb = varb - 1; // si?
            return this.obj.variables()[varb];
          }
          return varb
        }
        start (varb) {
          varb = this.convert(varb);
          return this.obj.start(varb)
        }
        end (varb) {
          varb = this.convert(varb);
          return this.obj.end(varb)
        }
        span (varb) {
          varb = this.convert(varb);
          return this.obj.span(varb);
        }
        group (varb) {
          varb = this.convert(varb);
          return this.obj.group(varb)
        }
        groups () {
          let content = [];
          let vars = this.obj.variables();
          for (let i = 0; i < vars.length; i++) {
            const varb = vars[i];
            content.push(this.obj.group(varb))
          }
          return content
        }
        groupdict () {
          let dic = {};
          let vars = this.obj.variables();
          for (let i = 0; i < vars.length; i++) {
            const varb = vars[i];
            dic[varb] = this.obj.group(varb);
          }
          return dic
        }
        
      }

      /*      Compile      */
      const compile = function(pattern, ...flags) {
        let rgxOptions = new RegExOptions();
        if (multi_line) {
          rgxOptions.multi_line = true;
        }
        if (early_output) {
          rgxOptions.early_output = true;
        }
        if (line_by_line) {
          rgxOptions.line_by_line = true;
        }
        if (dot_nl) {
          rgxOptions.dot_nl = true;
        }
        if (save_anchors) {
          rgxOptions.save_anchors = true;
        }
    
        let rgx = new Regex(pattern, rgxOptions);
        return rgx
      }
      let rgx = compile('.*!x{a}.*', multi_line=true, line_by_line=false);
      // console.log(rgx.find('aaaaaa'));
      // console.log(rgx.findIter('aaaaaa').next().value);
      // console.log(rgx.findIter('aaaaaa').next().value);
      // console.log(rgx.findIter('aaaaaa').next());
      // console.log(rgx.findIter('aaaaaa').next());
      // console.log(rgx.findIter('aaaaaa').next());
      // console.log(rgx.findIter('aaaaaa').next());
      // console.log(rgx.findIter('aaaaaa').next());
      // export {compile};
      /**/
      
    }
    catch (err) {
      console.log('Error: ', err);
    }
  }
  // module.exports = {
  //   hello: hello,
  //   compile: compile
  // }