//
//  TestView.swift
//  pshylosophia
//
//  Created by Елизавета Сенюк on 30.05.25.
//

import SwiftUI

struct TestView: View {
    @State private var currentQA = qaList.randomElement()!
    @State private var showAnswer = false
    
    var body: some View{
        VStack(spacing:30){
            Text(currentQA.question)
                .font(.headline)
                .multilineTextAlignment(.center)
            
            if showAnswer{
                Text(currentQA.answer)
                    .foregroundColor(.gray)
                    .multilineTextAlignment(.center)
            }
            Button (showAnswer ? "Следующий вопрос" : "Показать ответ" ){
                if showAnswer{
                    currentQA = qaList.randomElement()!
                }
                showAnswer.toggle()
            }
            .padding()
            .background(Color.blue.opacity(0.2))
            .cornerRadius(10)
        }
        .padding()
        .toolbar{
            ToolbarItem(placement: .principal) {
                Text("Тест")
                    .padding(.top, 10)
                    .font(.system(size: 30))
                    .fontDesign(.serif)
            }
            
        }
    }
}


