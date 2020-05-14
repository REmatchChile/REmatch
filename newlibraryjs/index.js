Module = require('./spanners_interface.js');

Module['onRuntimeInitialized'] = () => {
    let usedVars = [];
    let spanList = [];
    let curr;
    let allSpans = [];

    const instance = new Module.WasmInterface('aaaa', '.*!xd{a}!lol{a}.*');
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
    }
    instance.delete();
    console.log(allSpans, usedVars);
    console.log("%cHola", "color:red;");
}