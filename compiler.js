function lexer(input){
    const tokens = [];
    let cursor = 0; // cursor to keep track of the current position in the input string
    while(cursor < input.length){
        let char = input[cursor];

        //skip whitespace
        if (/\s/.test(char)){
            cursor++;
            continue;
        }
        if(/[a-zA-Z]/.test(char)){
            let word = '';
            while(/[a-zA-Z0-9]/.test(char)){
                word += char;
                char = input[++cursor];
            }
            if(word === 'dg' || word === 'dgpr'){
                tokens.push({type: 'keyword', value: word});
            }
        }
    }
}
function compiler(input){
    const token = lexer(input);
}
const code = `
dg x = 10
dg y = 20
dg z = x + y
dgpr z
`