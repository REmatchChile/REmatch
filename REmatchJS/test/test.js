const REmatch = require('../lib')
const fs = require('fs');
try {
    
    // Ejemplo find
    const text = 'Lorem ipsum dolor sit amet consectetur adipiscing elit euismod condimentum, conubia a mollis pulvinar nascetur taciti elementum praesent'
    let rgx1 = REmatch.compile('.*!word{dolor}.*')
    const result = rgx1.find(text).span('word')
    console.log(result)

    // Ejemplo finIter
    let rgx2 = REmatch.compile('.*!x{ly}.*');
    const text1 = "He was carefully disguised but captured quickly by police.";
    console.log(rgx2.findIter(text1).next());
    console.log(rgx2.findIter(text1).next());
    console.log(rgx2.findIter(text1).next());

    // Ejemplo findall
    const text2 = "He was carefully disguised but captured quickly by police.";
    let rgx3 = REmatch.compile('.*!adverb{ [a-zA-Z]+ly}.*');
    let list = rgx3.findall(text2);
    for (let index = 0; index < list.length; index++) {
      const element = list[index].group('adverb');
      console.log(element)
    }

    // Error caracteres extraños
    const correo = 'correo@gmail.com';
    let match2 = REmatch.find('.*!username{[^@]+}@!domain{[^@]+}\.[a-z]+.*', correo);
    console.log(match2.group('username'))

    // Error OOM
    const doc = fs.readFileSync('../../../../RKBExplorer/sparql.log.combo.1', "latin1");
    let match1 = REmatch.find('.*sparql!x{[^\n]*OPTIONAL[^\n]*\n}.*', doc);
    console.log(match1.group('x'));

    
} catch (error) {
    console.log(error)
}
