package rematch;

import rematch.*;
import java.util.Map;

public class Main {
    public static void main(String[] args) {
        try {

            System.out.println("\n============= QUERY TEST =============");

            String pattern = "!motif{F.{10}RE.{72,86}RD.K.{9}[CS]}";
            String document = """
            >XP_024849117.1 mitogen-activated protein kinase 10 isoform X5 [Bos taurus]
            MSKSKVDNQFYSVEVGDSTFTVLKRYQNLKPIGSGAQGIVCAAYDAVLDRNVAIKKLSRPFQNQTHAKRAYRELVLMKCVNHKNIISLLNVFTPQKTLEEFQDVYLVMELMDANLCQVIQMELDHERMSYLLYQMLCGIKHLHSAGIIHRDLKPSNIVVKSDCTLKILDFGLARTAGTSFMMTPYVVTRYYRAPEVILGMGYKENVDIWSVGCIMGEMVRHKILFPGRDYIDQWNKVIEQLGTPCPEFMKKLQPTVRNYVENRPKYAGLTFPKLFPDSLFPADSEHNKLKASQARDLLSKMLVIDPAKRISVDDALQHPYINVWYDPAEVEAPPPQIYDKQLDEREHTIEEWKELIYKEVMNSEEKTKNGVVKGQPSPSAQVQP
            """;

            Query query = new Query(pattern, Flags.none());

            System.out.println("\nQUERY::CHECK()");
            System.out.println("¿Documento válido? " + query.check(document));

            System.out.println("\nQUERY::VARIABLES():");
            for (String var : query.variables()) {
                System.out.println("→ " + var);
            }

            
            System.out.println("\nQUERY::FINDONE()");
            Match firstMatch = query.findOne(document);
            System.out.println("Empty: " + firstMatch.empty());
            System.out.println("Match.toString(): " + firstMatch.toString());
            for (String var : firstMatch.variables()) {
                System.out.println("Grupo': " + firstMatch.group(var));
                System.out.println("Start: " + firstMatch.start(var));
                System.out.println("End: " + firstMatch.end(var));
            }
            Map<String, javacpp.Span> groupDict = firstMatch.groupdict();
            System.out.println("Groupdict:");
            for (Map.Entry<String, javacpp.Span> entry : groupDict.entrySet()) {
                String key = entry.getKey();
                javacpp.Span span = entry.getValue();
                System.out.println("→ " + key + ": (" + span.first() + ", " + span.second() + ")");
            }

            
            System.out.println("\nQUERY::FINDALL()");
            for (Match match : query.findAll(document)) {
                System.out.println("→ " + match.toString());
            }

            System.out.println("\nQUERY::FINDMANY(2)");
            for (Match match : query.findMany(document, 2)) {
                System.out.println("→ " + match.toString());
            }


            System.out.println("\nQUERY::FINDITER()");
            // forma correcta de iterar 
            MatchGenerator matchGenerator = query.findIter(document);
            MatchGeneratorIterator it = matchGenerator.iterator();
            System.out.println("Iterando matches encontrados...");
    
            while (it.hasNext()){
                Match match = it.operatorStar();
                System.out.println("————— "+ match.toString());
                it.next();
            }



            System.out.println("\n============= MULTIQUERY TEST =============");

            String patternMulti = "(public|private)\\s+\\w+\\s+!fun{\\w+}\\(([^,)]+,){2,}[^,)]+\\)";
            String documentMulti = 
            """
            public class Game
            {
                public readonly Deck deck;
                public readonly Card card;
                private readonly List<Player> players;
            
                public void Play1(Player p1, Card card) {
                    players.Add(p1);
                    this.card = card;
                }
                public void Play2(Player p1, Player p2, Card card) {
                    players.Add(p1);
                    players.Add(p2);
                    this.card = card;
                }
                public void Play() {
                    card = null;
                }
            }
            """;

            MultiQuery multiQuery = new MultiQuery(patternMulti, Flags.none());

            System.out.println("\nMULTIQUERY::CHECK()");
            System.out.println("¿Documento válido? " + multiQuery.check(documentMulti));

            System.out.println("\nMULTIQUERY::VARIABLES()");
            for (String var : multiQuery.variables()) {
                System.out.println("→ " + var);
            }
            
            System.out.println("\nMULTIQUERY::FINDONE()");
            MultiMatch first = multiQuery.findOne(documentMulti);
            System.out.println("→ Match vacío: " + first.empty());
            System.out.println("→ Match.toString(): " + first.toString());

            int variableId = 0; // ID de la variable a buscar
            for (String var : first.variables()) {
                System.out.println("→ VAR '" + var + "' → SPANS: " + first.spans(var).size());
                System.out.println("→ GRUPOS: " + first.groups(var));
                variableId++;
            }
            

            System.out.println("—— MULTIMATCH::OPERATOREQUALS()");

            MultiMatch match1 = multiQuery.findOne(documentMulti);
            MultiMatch match2 = multiQuery.findOne(documentMulti);
            System.out.println("————— match1 == match2: " + match1.equals(match1)); // Esperado: true


            System.out.println("\nMULTIQUERY::FINDALL()");
            for (MultiMatch m : multiQuery.findAll(documentMulti)) {
                System.out.println("→ " + m.toString());
            }

            System.out.println("\nMULTIQUERY::FINDMANY(2)");
            for (MultiMatch m : multiQuery.findMany(documentMulti, 2)) {
                System.out.println("→ " + m.toString());
            }

            System.out.println("\nMULTIQUERY::FINDITER()");
            MultiMatchGenerator multiMatchGenerator = multiQuery.findIter(documentMulti);
            MultiMatchGeneratorIterator itMulti = multiMatchGenerator.iterator();

            while (itMulti.hasNext()) {
                MultiMatch multiMatch = itMulti.operatorStar();
                System.out.println("————— "+ multiMatch.toString());
                itMulti.next();
            }
            

            

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
