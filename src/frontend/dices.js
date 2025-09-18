function generate_dices() {

}
fetch("test_build/json/1234.json") 
    .then(res => res.json())
    .then(data => {
        document.getElementById("dices1").innerHTML = `
        <table>
            <tr>
                <th>${data.dice1}</th>
                <th>${data.dice2}</th>
                <th>${data.dice3}</th>
                <th>${data.dice4}</th>
                <th>${data.dice5}</th>
            </tr>
        </table>
        `;
    })
    .catch(err => console.error("JSON fail", err));