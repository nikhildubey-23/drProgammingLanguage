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

function parser(tokens){
    const ast = {
        type: 'Program',
        body: []
    }
    while(tokens.length>0){
        let token = tokens.shift();
        if (token.type === 'keyword' && token.value === 'dg'){
            let declaration = {
                type : 'Declaration',
                name: tokens.shift().value,
                value: null
            }
            //check for the assignment 
            if (tokens[0].type === 'operator' && tokens[0].value === '='){
                tokens.shift();
                //parse the expression
                let expression = '';
                while(tokens.length > 0 && tokens[0].type !== 'keyword'){
                    expression += tokens.shift().value;
                }
                declaration.value = expression.trim();
            }
            ast.body.push(declaration);
        } else if (token.type === 'keyword' && token.value === 'dgpr') {
            let printStatement = {
                type: 'PrintStatement',
                value: tokens.shift().value
            }
            ast.body.push(printStatement);
        }
    }
    return ast;
}

function codeGen(node){
    switch(node.type){
        case 'Program':return node.body.map(codeGen).join('\n')
        case 'Declaration': return `const ${node.name} = ${node.value};`
        case 'PrintStatement': return `console.log(${node.value});`
    }
}

function compiler(input){
    const tokens = lexer(input);
    // console.log(tokens); //to check how the tokens work
    const ast = parser(tokens); //abstract syntax tree 
    // console.log(ast) // to the abstract syntax tree 
    const executableCode = codeGen(ast)
    //console.log(executableCode); //to see the generated code
    return executableCode
}

function runner(input){
    eval(input)
}
const code = `
dg x = 10
dg y = 20
dg z = x + y
dgpr z
`
const exec = compiler(code);
runner(exec);