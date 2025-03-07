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
            }else{
                tokens.push({type: 'identifier', value: word});
            }
            continue
        }
        if (/[0-9]/.test(char)){
            let number = '';
            while(/[0-9]/.test(char)){
                number += char;
                char = input[++cursor];
            }
            tokens.push({type: 'number', value: number});
            continue;
        }
        //tokenize operators and equals sign
        if(/[\+\-\*\/\=]/.test(char)){
            tokens.push({type: 'operator', value: char});
            cursor++;
            continue;
        }
    }
    return tokens;
}
function compiler(input){
    const token = lexer(input);
    // console.log(token); //to check how the tokens work
}
const code = `
dg x = 10
dg y = 20
dg z = x + y
dgpr z
`
compiler(code);