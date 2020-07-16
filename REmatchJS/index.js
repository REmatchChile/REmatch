const XRegExp = require('xregexp');
const Module = require('../REmatchEngine/bin/rematch_wasm.js');
const fs = require('fs');
var readline = require('readline-sync');


Module['onRuntimeInitialized'] = () => {

    try {
        const f = (text, regex) => {
        	console.log(regex, text.slice(0, 50))
        	// text = text.slice(0, 800)
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
            console.log('Total outputs: ', allSpans.length);
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
            query = new RegExp(query, 'g')
            const exp = doc.match(query);
            let end = Date.now();
            const time = end - start;
            console.log('Time String: ', time);
            console.log('Total outputs: ', exp.length);
            return time

        }


        const queries_rm = ['.*sparql!x{[^\n]*OPTIONAL[^\n]*\n}.*', 
        '.*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*',
        '.*.................12[^\n]*sparql!x{[^\n]*FILTER[^\n]*\n}.*',
        '.*!y{[^\+]*\+}[^\n]*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*'];

        const queries_rx = ['sparql([^\n]*OPTIONAL[^\n]*\n)', 
        'sparql([^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n)',
        '.................12[^\n]*sparql([^\n]*FILTER[^\n]*\n)',
        '([^\+]*\n)[^\+]*sparql([^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n)'];


        const test_a_file = (combo, nro_query, iterations) => {
            
            query_rm = queries_rm[nro_query]
            query_rx = queries_rx[nro_query]
         

            // const doc = fs.readFileSync('../../RKBExplorer/sparql.log.combo.' + combo, "latin1");
            const doc = 'kjslad OPTIONAL jkjsdkjahds \
            lasdkas'
            // REmatch
            let times = [];
            console.log("---" + "Iniciando REmatch" + "---" );

            try {
                for (var i = 0; i <= iterations - 1; i++) {
                    let exp = f(doc, query_rm);
                    times.push(exp);
                }
            }
            catch (err) {
                console.log('Error: ', err)
            }
            
            // XRegExp
            let times_XRegExp = []
            console.log("---" + "Iniciando XRegExp" + "---");

            for (var i = 0; i <= iterations - 1; i++) {
                let exp = XRegExp_exp(doc, query_rx);
                times_XRegExp.push(exp);
            }
            
            // String
            let times_String = []
            console.log("---" + "Iniciando String" + "---");

            
            for (var i = 0; i <= iterations - 1; i++) {
                let exp = Str_exp(doc, query_rx);
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

        const test_all_files = (nro_query, iterations) => {
            for (var i = 0; i <= 10; i++) {
                    combo = (i + 1).toString();
                    console.log('*************', combo, '*************')
                    test = test_a_file(combo, nro_query, iterations)
                    
                    
                }
        }

        const test_all_queries = (combo, iterations) => {
            for (var i = 0; i <= 3; i++) {
                test = test_a_file(combo, i, iterations)
            }
        }
        
		var choice = readline.question("[0] Test all files\n[1] Test a file\n[2] Test all queries\n>");

		if (choice == '0') {
			console.log('[0] sparql!x{[^\\n]*OPTIONAL[^\\n]*\\n}')
			console.log('[1] sparql!x{[^\\n]*OPTIONAL[^\\n]*OPTIONAL[^\\n]*\\n}')
			console.log('[2] .................12[^\\n]*sparql!x{[^\\n]*FILTER[^\\n]*\\n}')
			console.log('[3] !y{[^\+]*\+}[^\\n]*sparql!x{[^\\n]*OPTIONAL[^\\n]*OPTIONAL[^\\n]*\\n}')
			var query = readline.question("Ingrese consulta: ");
			var iterations = Number(readline.question("Iteraciones: "));
			test_all_files(query, iterations);

		} 
		if (choice == '1') {

			var combo = readline.question("Ingrese el número del combo: ");
			console.log('[0] sparql!x{[^\\n]*OPTIONAL[^\\n]*\\n}')
			console.log('[1] sparql!x{[^\\n]*OPTIONAL[^\\n]*OPTIONAL[^\\n]*\\n}')
			console.log('[2] .................12[^\\n]*sparql!x{[^\\n]*FILTER[^\\n]*\\n}')
			console.log('[3] !y{[^\+]*\+}[^\\n]*sparql!x{[^\\n]*OPTIONAL[^\\n]*OPTIONAL[^\\n]*\\n}')
			var query = readline.question("Ingrese consulta: ");
			var iterations = Number(readline.question("Iteraciones: "));
			test_a_file(combo, query, iterations);
		}
		else {
			var combo = readline.question("Ingrese el número del combo: ");
			var iterations = Number(readline.question("Iteraciones: "));
			test_all_queries(combo, iterations);
		}

    }
    catch(err) {
        console.log('Error: ', err)
    }
   
    }



