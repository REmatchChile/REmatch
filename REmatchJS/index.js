const XRegExp = require('xregexp');
const Module = require('./spanners_interface.js');
const fs = require('fs');


Module['onRuntimeInitialized'] = () => {

    try {
        const f = (text, regex) => {
            let start = Date.now();
            let usedVars = [];
            let spanList = [];
            let curr;
            let allSpans = [];
            
            const instance = new Module.WasmInterface(text, regex);
            instance.init();
            let schema = instance.getOutputSchema();
            for (let i = 0; i < schema.size(); i++) {
                usedVars.push(schema.get(i));   
            }

            while (instance.hasNext()) {
                spanList = [];
                curr = instance.next();
                for (i = 0; i < usedVars.length; i++) {
                    spanList.push(curr.get(i));
                }
                allSpans = allSpans.concat([spanList]);
                instance.next();
                
            }
            instance.delete();
            var end = Date.now();
            const time = end - start
            console.log('Time REmatch: ', time)
            console.log('Outputs: ', allSpans.length);
            return time
        }

        const XRegExp_exp = (doc, query) => {
            let start = Date.now();
            const exp = XRegExp.match(doc, XRegExp(query, 'g'));
            let end = Date.now();
            const time = end - start;
            console.log('Time XRegExp: ', time);
            console.log('Total outputs: ', exp.length);
            return time

        }

        const Str_exp = (doc, query) => {
            let start = Date.now();
            const exp = doc.match(query);
            let end = Date.now();
            const time = end - start;
            console.log('Time String: ', time);
            console.log('Total outputs: ', exp.length);
            return time

        }


        const queries = ['.*sparql!x{[^\n]*OPTIONAL[^\n]*\n}.*', 
        '.*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*',
        '.*.................12[^\n]*sparql!x{[^\n]*FILTER[^\n]*\n}.*',
        '.*!y{[^\+]*\+}[^\n]*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*'];

        

        const test_a_file = (combo, query, iterations) => {
            if (query == 0) {
                let query = queries[2]
            }

            const doc = fs.readFileSync('RKBExplorer/sparql.log.combo.' + combo, "latin1");
            
            // REmatch
            // let rows = [];
            let times = [];
            console.log("---" + "Iniciando REmatch" + "---" );

            try {
                for (var i = 0; i <= iterations - 1; i++) {
                    let exp = f(doc, query);
                    times.push(exp);
                }
            }
            catch (err) {
                console.log(err)
            }
            
            // XRegExp
            let times_XRegExp = []
            console.log("---" + "Iniciando XRegExp" + "---");

            for (var i = 0; i <= iterations - 1; i++) {
                let exp = XRegExp_exp(doc, '[^\n]*OPTIONAL[^\n]*\n');
                times_XRegExp.push(exp);
            }
            
            // String
            let times_String = []
            console.log("---" + "Iniciando String" + "---");

            
            for (var i = 0; i <= iterations - 1; i++) {
                let exp = Str_exp(doc, /[^\n]*OPTIONAL[^\n]*\n/g);
                times_String.push(exp);
            }
            console.log('-------------------------')
            const count_times = [[times, 'REmatch'], [times_String, 'String'], 
            [times_XRegExp, 'XRegExp']]
            
            for (var i = count_times.length - 1; i >= 0; i--) {
                var sum = count_times[i][0].reduce(function(a, b){return a + b}, 0);
                const avg = sum / count_times[i][0].length;
                console.log('Average ' + count_times[i][1] + ': ', avg);
            }
        }

        const test_all_combos = (query, iterations) => {
            for (var i = 0; i <= 10; i++) {
                    combo = (i + 1).toString();
                    test = test_a_file(combo, query, iterations)
                }
        }

        const test_all_queries = (combo, iterations) => {
            for (var i = 0; i <= queries.length - 1; i++) {
                    query = queries[i];
                    test = test_a_file(combo, query, iterations)
                }
        }
        test_a_file('1', 0, 1)
    }
    catch(err) {
        console.log(err)
    }
   
    }



