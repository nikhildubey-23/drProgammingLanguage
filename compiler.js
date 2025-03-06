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