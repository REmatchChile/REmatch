importScripts('./spanners_interface.js');
var schema = [];
var spanList = [];
var curr;
var allSpans = [];

Module['onRuntimeInitialized'] = () => {
    onmessage = (m) => {
        try {
            var t0 = Date.now();
            var instance = new Module.WasmInterface(m.data.text, m.data.query);
            instance.init();
            var tempSchema = instance.getOutputSchema();
            for (var i = 0; i < tempSchema.size(); i++) {
                schema.push(tempSchema.get(i));
            }
            while (instance.hasNext()) {
                spanList = [];
                curr = instance.next();
                for (i = 0; i < schema.length; i++) {
                    spanList.push(curr.get(i));
                }
                allSpans = allSpans.concat([spanList]);
            }
            instance.delete(); // Necessary?
            postMessage(
                {   
                    schema: schema,
                    spans: allSpans, 
                    log: `${allSpans.length} results in ${(Date.now() - t0) / 1000}s.`,
                }
            )
        } catch(err) {
            instance.delete(); // Necessary?
            postMessage(
                {
                    error: 'Out of memory! The worker will be restarted.',
                }
            ) 
        }
    }
}